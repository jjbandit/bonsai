#define BONSAI_NO_PUSH_METADATA

#include <bonsai_types.h>
#include <unix_platform.cpp>

#include <debug_data_system.cpp>

#include <test_utils.cpp>

global_variable memory_arena* TranArena = PlatformAllocateArena();
#include <bonsai_string.cpp>

void
TestPopWord(ansi_stream *Stream, memory_arena *Memory)
{
  {
    char *Word = PopWord(Stream, Memory);
    TestThat(StringsMatch(Word, "word1"));
  }
  {
    char *Word = PopWord(Stream, Memory);
    TestThat(StringsMatch(Word, "word2"));
  }
  {
    char *Word = PopWord(Stream, Memory);
    TestThat(StringsMatch(Word, "word3"));
  }

  return;
}

void
TestPopU32(ansi_stream *Stream, memory_arena *Memory)
{
  {
    u32 N = PopU32(Stream, Memory);
    TestThat(N == 1);
  }
  {
    u32 N = PopU32(Stream, Memory);
    TestThat(N == 2);
  }
  {
    u32 N = PopU32(Stream, Memory);
    TestThat(N == 3);
  }

  return;
}

void
TestStreamCursor()
{
  memory_arena *Memory = PlatformAllocateArena(Megabytes(1));

  {
    const char *TestData = "word1 word2 word3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopWord(&Stream, Memory);
  }

  {
    const char *TestData = "word1\nword2\nword3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopWord(&Stream, Memory);
  }

  {
    const char *TestData = "word1    word2\nword3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopWord(&Stream, Memory);
  }

  {
    const char *TestData = "  word1    word2\nword3  ";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopWord(&Stream, Memory);
  }


  {
    const char *TestData = "1 2 3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopU32(&Stream, Memory);
  }

  {
    const char *TestData = "1\n2\n3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopU32(&Stream, Memory);
  }

  {
    const char *TestData = "1    2\n3";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopU32(&Stream, Memory);
  }

  {
    const char *TestData = "  1    2\n3  ";
    ansi_stream Stream = AnsiStream(TestData);
    TestPopU32(&Stream, Memory);
  }

  {
    const char *TestData = "1/2/3";
    ansi_stream Stream = AnsiStream(TestData);
    {
      u32 N = PopU32(&Stream, Memory, "/");
      TestThat(N == 1);
    }
    {
      u32 N = PopU32(&Stream, Memory, "/");
      TestThat(N == 2);
    }
    {
      u32 N = PopU32(&Stream, Memory);
      TestThat(N == 3);
    }
  }

  {
    const char *TestData = "1//3//";
    ansi_stream Stream = AnsiStream(TestData);
    {
      u32 N = PopU32(&Stream, Memory, "/");
      TestThat(N == 1);
    }
    {
      u32 N = PopU32(&Stream, Memory);
      TestThat(N == 3);
    }
  }


  return;
}

void
TestContains()
{
  TestThat( Contains("abc", "abc") );
  TestThat( Contains("abcd", "abc") );
  TestThat( Contains("dabc", "abc") );
  TestThat( Contains("dabcd", "abc") );

  TestThat(!Contains("bc", "abc") );
  TestThat(!Contains("bacd", "abc") );
  TestThat(!Contains("dacb", "abc") );
  TestThat(!Contains("adbcd", "abc") );

  return;
}

void
TestReadUntilTerminatorList()
{
  const char *Text = "property='> some thing >'";


  {
    ansi_stream Stream = AnsiStream(Text);

    counted_string Result = ReadUntilTerminatorList(&Stream, "'");
    TestThat(Result.Start[Result.Count] == '\'');
    TestThat(Stream.At[0] == '>');
  }

  {
    ansi_stream Stream = AnsiStream(Text);

    counted_string Result = ReadUntilTerminatorList(&Stream, "=> \"'");
    TestThat(Result.Start[Result.Count] == '=');
    TestThat(Stream.At[0] == '\'');
  }

  {
    ansi_stream Stream = AnsiStream(Text);

    counted_string Result = ReadUntilTerminatorList(&Stream, "pr");
    TestThat(Result.Count == 0);
    TestThat(Result.Start[Result.Count] == 'p');
    TestThat(Stream.At[0] == 'r');
  }

  return;
}

s32
main()
{
  TestSuiteBegin("String");
  {
    const char *Test1 = "v";
    const char *Test2 = "v";
    TestThat(StringsMatch(Test1, Test2));
  }

  {
    const char *Test1 = "vv";
    const char *Test2 = "vv";
    TestThat(StringsMatch(Test1, Test2));
  }

  {
    const char *Test1 = "v v";
    const char *Test2 = "v v";
    TestThat(StringsMatch(Test1, Test2));
  }

  {
    const char *Test1 = "vv";
    const char *Test2 = "v";
    TestThat(!StringsMatch(Test1, Test2));
  }

  {
    const char *Test1 = "v";
    const char *Test2 = "vv";
    TestThat(!StringsMatch(Test1, Test2));
  }

  TestStreamCursor();

  TestContains();

  TestReadUntilTerminatorList();

  TestSuiteEnd();
  exit(TestsFailed);
}

