inline void
PrintSemValue(semaphore *Semaphore)
{
  s32 SemValue;
  if (sem_getvalue(Semaphore, &SemValue) == 0)
  {
    Print(SemValue);
  }
  else
  {
    Warn("sem_getvalue() failed.");
  }
  return;
}


void
PlatformInitializeMutex(mutex *Mutex)
{
  pthread_mutex_init(&Mutex->M, NULL);
  return;
}

void
PlatformUnlockMutex(mutex *Mutex)
{
  s32 Fail = pthread_mutex_unlock(&Mutex->M);
  TIMED_MUTEX_RELEASED(Mutex);

  if (Fail)
  {
    Error("Failed to un-lock mutex");
    Assert(False);
  }

  return;
}

void
PlatformLockMutex(mutex *Mutex)
{
  TIMED_MUTEX_WAITING(Mutex);

  s32 Fail = pthread_mutex_lock(&Mutex->M);

  TIMED_MUTEX_AQUIRED(Mutex);

  if (Fail)
  {
    Error("Failed to aquire lock");
    Assert(False);
  }

  return;
}

bonsai_function u64
PlatformGetPageSize()
{
  u64 InvalidSysconfReturn = ((u64)-1);
  local_persist u64 PageSize = (u64)sysconf(_SC_PAGESIZE);
  Assert(PageSize != InvalidSysconfReturn);
  Assert(PageSize == 4096);
  return PageSize;
}

u32
GetLogicalCoreCount()
{
  local_persist u32 CoreCount = (u32)sysconf(_SC_NPROCESSORS_ONLN);
  return CoreCount;
}

u8*
PlatformProtectPage(u8* Mem)
{
  u64 PageSize = PlatformGetPageSize();
  Assert((u64)Mem % PageSize == 0);
  s32 ProtectSuccess = (mprotect(Mem, PageSize, PROT_NONE) == 0);

  u8 *Result = 0;
  if (ProtectSuccess)
  {
    Result = Mem;
  }
  else
  {
    if (errno == EACCES)
    {
      Error("EACCES");
    }

    if (errno == EINVAL)
    {
      Error("EINVAL");
    }

    if (errno == ENOMEM)
    {
      Error("ENOMEM");
    }

    Error("mprotect failed");
    PlatformDebugStacktrace();
    Assert(False);
  }

  return Result;
}

void
PlatformDeallocateArena(memory_arena *Arena)
{
  if (Arena->Start)
  {
    s32 Deallocated = (munmap(Arena->Start, TotalSize(Arena)) == 0);
    Assert(Deallocated);
  }

#if MEMPROTECT_OVERFLOW
  {
    umm PageSize = PlatformGetPageSize();
    u8 *ArenaBytes =  (u8*)Arena - ((umm)Arena % PageSize);
    s32 Deallocated = (munmap(ArenaBytes, PageSize*2) == 0);
    Assert(Deallocated);
  }
#else
  NotImplemented;
#endif

  return;
}

u8*
PlatformAllocateSize(umm AllocationSize)
{
  Assert(AllocationSize % PlatformGetPageSize() == 0);

  s32 Protection = PROT_READ|PROT_WRITE;
  s32 Flags = MAP_SHARED|MAP_ANONYMOUS|MAP_NORESERVE;

  u8 *Bytes = (u8*)mmap(0, AllocationSize, Protection, Flags,  -1, 0);
  if (Bytes == MAP_FAILED)
  {
    Bytes = 0;
    s32 E = errno;
    if (E == ENOMEM)
    {
      Error("Out of memory, or exhausted virtual page table.");
      Assert(False);
    }
    else
    {
      Error("Unknown error allocating pages: %d", E);
      Assert(False);
    }
  }

  return Bytes;
}

global_variable volatile b32 MainThreadBlocksWorkerThreads;
global_variable volatile u32 WorkerThreadsWaiting;

#if PLATFORM_THREADING_IMPLEMENTATIONS

inline void
ThreadSleep( semaphore *Semaphore )
{
  TIMED_FUNCTION();

  AtomicIncrement(&WorkerThreadsWaiting);
  sem_wait(Semaphore);
  AtomicDecrement(&WorkerThreadsWaiting);
  return;
}

semaphore
CreateSemaphore(void)
{
  semaphore Result;
  sem_init(&Result, 0, 0);

  return Result;
}

// TODO(Jesse id: 230, tags: parsing, metaprogramming, high_priority): ParseVariable should work with bonsai_function pointer types
typedef void* (*thread_main_callback_type)(void*);

typedef pthread_t thread_handle; // TODO(Jesse id: 265): Unnecessary .. I just added it as a hack get parsing to work
typedef pthread_attr_t thread_attributes; // TODO(Jesse id: 266): Unnecessary .. I just added it as a hack get parsing to work

thread_id
CreateThread( thread_main_callback_type ThreadMain, thread_startup_params *Params)
{
  thread_attributes Attribs;
  pthread_attr_init(&Attribs);

  thread_handle Thread;
  Params->Self.ID = pthread_create(&Thread, &Attribs, ThreadMain, Params);

  return Params->Self.ID;
}

#endif // PLATFORM_THREADING_IMPLEMENTATIONS

char*
GetCwd()
{
  // @memory-leak
  // FIXME(Jesse): get_current_dir_name mallocs interally .. do we care?
  global_variable char *Result = get_current_dir_name();
  return (Result);
}

b32
IsFilesystemRoot(char *Filepath)
{
  b32 Result = ( Filepath[0] == '/' && Filepath[1] == 0 );
  return Result;
}

// TODO(Jesse id: 269): Unnecessary .. I just added these as a hack get parsing to work
typedef timespec bonsai_timespec;

inline r64
GetHighPrecisionClock()
{
  bonsai_timespec Time;
  clock_gettime(CLOCK_MONOTONIC, &Time);

  r64 SecondsAsMs = (r64)Time.tv_sec*1000.0;
  r64 NsAsMs = Time.tv_nsec/1000000.0;

  r64 Ms = SecondsAsMs + NsAsMs;
  return Ms;
}
// TODO(Jesse id: 270): Unnecessary .. I just added these as a hack get parsing to work
typedef sched_param bonsai_sched_param;

inline void
PlatformSetThreadPriority(s32 Priority)
{
  bonsai_sched_param Param = {};
  Param.sched_priority = Priority;
  s32 E = sched_setscheduler(0, SCHED_FIFO, &Param);
  if (E)
  {
    Error("Setting Scheduler for main thread");

    switch (errno)
    {
      case EINVAL:
      {
        Error(" Invalid arguments: pid is negative or param is NULL.");
        Error(" (sched_setscheduler()) policy is not one of the recognized policies.");
        Error(" (sched_setscheduler()) param does not make sense for the specified policy.");
      }  break;

      case EPERM:  { Error("  The calling thread does not have appropriate privileges."); break; }
      case ESRCH:  { Error("  The thread whose ID is pid could not be found."); break; }
      InvalidDefaultCase;
    }
  }

  return;
}

// It seemed to me doing this actually made performance _worse_
#if 0

// TODO(Jesse id: 271): Unnecessary .. I just added these as a hack get parsing to work
typedef cpu_set_t bonsai_cpu_set_t;

inline void
PlatformSetMainThreadPriority()
{
  bonsai_cpu_set_t Cpu;
  CPU_ZERO(&Cpu);
  CPU_SET(0, &Cpu);

/*   int SetSuccessful = pthread_setaffinity_np(pthread_self(), sizeof(Cpu), &Cpu); */
/*   if (SetSuccessful == -1) */
/*   { */
/*     Error("Setting CPU affinity"); */
/*   } */

  /* PlatformSetThreadPriority(99); */
  return;
}

inline void
PlatformSetWorkerThreadPriority()
{
  bonsai_cpu_set_t Cpu;
  CPU_ZERO(&Cpu);
  CPU_SET(1, &Cpu);

  /* int SetSuccessful = pthread_setaffinity_np(pthread_self(), sizeof(Cpu), &Cpu); */
  /* if (SetSuccessful == -1) */
  /* { */
  /*   Error("Setting CPU affinity"); */
  /* } */

  /* PlatformSetThreadPriority(90); */
  return;
}
#endif

