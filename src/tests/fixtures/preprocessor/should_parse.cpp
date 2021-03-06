#include <src/tests/fixtures/preprocessor/other_file_that_should_parse.cpp>

#define VariadicMacroFunction3(...) __VA_ARGS__
VariadicMacroFunction3(A, B, C, D);

#define __QWORD unsigned long long
#define __STD_TYPE typedef
#define __DEV_T_TYPE __QWORD
#define __FSID_T_TYPE struct { int __val[2]; }

__STD_TYPE __FSID_T_TYPE __fsid_t;
__STD_TYPE __DEV_T_TYPE __dev_t;



#define __THING int
#define __INDIRECT_THING_TYPE_NAME __alias_for_int
#define __THING_TYPE_NAME __INDIRECT_THING_TYPE_NAME

__STD_TYPE __THING __THING_TYPE_NAME;




#define EXTERN_BLOCK extern "C" {
#define EXTERN_BLOCK_END }

// This is buggy
//
/* EXTERN_BLOCK */

/* #include <some_other_file_that_wants_c_linkage.h> */

/* u32 */
/* ExternalFunc() */
/* { */
/* } */

/* EXTERN_BLOCK_END */



_Pragma("whatever")
_Pragma("whatever");

__pragma(msvc_style(thing));


#define bonsai_function static

// Buggy comma operator cases
// int x, y;

// int u = x < 8 ? x : y,
//     v = h < 4 ? y : x == 12 || y == 14 ? x : y;

unsigned int Int0 = 42;
// unsigned     Int1 = 42; // This needs to be fixed in the parser
int          Int2 = 42;
signed int   Int3 = 42;
// signed       Int4 = 42; // This needs to be fixed in the parser




typedef unsigned int u32;
u32 SizeOfInt = sizeof(int);



#define MacroFunction(P1, P2) P1 P2
MacroFunction(u32 AnUnsignedInt = 42;, )
MacroFunction(u32 AnotherUnsignedInt = sizeof(u32);, )

#define VariadicMacroFunction1(P1, P2, ...) P1 P2 __VA_ARGS__
#define VariadicMacroFunction2(P1, P2 ...) P1 P2 __VA_ARGS__



#if 0
// #define CheekyDisappearingFunction(...)

#else

bonsai_function u32
CheekyDisappearingFunction()
{
  return 42;
}

/* bonsai_function u32 */
/* CallTheCheekyFunction() */
/* { */
/*   u32 CheekyResult = CheekyDisappearingFunction(); */
/*   return CheekyResult; */
/* } */

#endif

/* #if 1 */
/* #define AnotherCheekyDisappearingFunction(...) */
/* #else */
/* bonsai_function u32 */
/* AnotherCheekyDisappearingFunction() */
/* { */
/*   return 42; */
/* } */

/* bonsai_function u32 */
/* CallTheAnotherCheekyFunction() */
/* { */
/*   u32 AnotherCheekyResult = AnotherCheekyDisappearingFunction(); */
/*   return AnotherCheekyResult; */
/* } */
/* #endif */


/* bonsai_function void */
/* CallBothCheeks() */
/* { */
/*   CheekyDisappearingFunction(); */
/*   AnotherCheekyDisappearingFunction(); */
/* } */




std::vector<int> IntVector;

struct thing
{
  std::vector<int> AnotherVector;
};


thing::thing() // Constructor bonsai_function
{
  std::vector<int> IntVectorInAFunction;
}

// Apparently constructor functions are allowed to have template arguments ..
// so this is legal if thing takes a template parameter
thing::thing<int>()
{
}

int
thing::DoTheThing(unsigned int Count) // Member bonsai_function
{
  int Result = (int)Count;
  return Result;
}

bonsai_function thing
TestFunc0()
{
  thing Result = {};
  return (Result);
}

void
TestFunc1()
{
  bool whatever = true;

  if (whatever)
  {
    /* This should
     * parse
     */
#define foo
  }

  if (whatever)
  {
    // This does parse
#define bar
  }

  int Six = 3 + 3;
}
