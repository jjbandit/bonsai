
meta(generate_stream(counted_string))
#include <metaprogramming/output/generate_stream_counted_string>
meta(generate_cursor(counted_string))
#include <metaprogramming/output/generate_cursor_counted_string>

// TODO(Jesse): Generate this?  Need a compiler feature to generate stuff from primitive types.
// @compiler-feature
struct char_cursor
{
  char* Start;
  char* End;
  char* At;
  memory_arena* Memory; // TODO(Jesse): Do we actually want this in here?
};

// Note(Jesse): Shamelessly copied from the Handmade Hero codebase
global_variable char DecChars[] = "0123456789";
global_variable char LowerHexChars[] = "0123456789abcdef";
global_variable char UpperHexChars[] = "0123456789ABCDEF";

umm
Length(const char *Str)
{
  const char *Start = Str;
  const char *End = Str;

  while (*End++);

  umm Result = (umm)(End - Start) - 1;
  return Result;
}

counted_string
CountedString(const char *S)
{
  counted_string Result = {};
  Result.Start = S;
  Result.Count = Length(S);
  return Result;
}

counted_string
CS(const char *S)
{
  counted_string Result = CountedString(S);
  return Result;
}

b32
Contains(counted_string Haystack, char Needle)
{
  b32 Result = False;
  for (u32 Index = 0;
      Index < Haystack.Count;
      ++Index)
  {
    Result |= (Needle == Haystack.Start[Index]);
  }
  return Result;
}

b32
Contains(const char* Haystack, char Needle)
{
  b32 Result = Contains(CS(Haystack), Needle);
  return Result;
}

// TODO(Jesse): Profile and check collision rate of this!
// @optimize
inline umm
StringHash(const char* S1)
{
  umm Result = 0x4fa691fd6a2c2f69;
  while (*S1++)
  {
    umm Char = (umm)*S1;
    umm Mix = Char | (Char<<8) | (Char<<16) | (Char<<24) | (Char<<32) | (Char<<40) | (Char<<48) | (Char<<56);
    Result ^= ((Result<<32) | (Result>>32)) & Mix;
  }

  return Result;
}

bonsai_function b32
IsPathSeparator(char C)
{
  b32 Result = C == '/';
  return Result;
}

bonsai_function counted_string
StripExtension(counted_string FilePath)
{
  counted_string Result = FilePath;

  for (umm CharIndex = 0;
      CharIndex < Result.Count;
      ++CharIndex)
  {
    if (Result.Start[CharIndex] == '.')
    {
      Result.Count = CharIndex;
      break;
    }
  }

  return Result;
}

bonsai_function counted_string
Basename(counted_string FilePath)
{
  umm LastPathSeparator = 0;
  for (umm CharIndex = 0;
      CharIndex < FilePath.Count;
      ++CharIndex)
  {
    if (IsPathSeparator(FilePath.Start[CharIndex]))
    {
      LastPathSeparator = CharIndex;
    }
  }
  counted_string Result = {
    .Count = FilePath.Count - LastPathSeparator,
    .Start = FilePath.Start + LastPathSeparator
  };

  return Result;
}

inline b32
StringsMatch(const char *S1, const char *S2)
{
  b32 Result = S1 && S2;
  while (Result && (*S1 || *S2))
  {
    Result &= (*S1++ == *S2++);
  }

  return Result;
}

inline b32
StringsMatch(counted_string* S1, counted_string* S2)
{
  b32 Result = True;

  if (S1 && S2 && S1->Count == S2->Count)
  {
    for (u32 CharIndex = 0;
        CharIndex < S1->Count;
        ++CharIndex)
    {
      Result = (Result && (S1->Start[CharIndex] == S2->Start[CharIndex]));
    }
  }
  else
  {
    Result = False;
  }

  return Result;
}

inline b32
StringsMatch(counted_string S1, counted_string* S2)
{
  b32 Result = StringsMatch(&S1, S2);
  return Result;
}

inline b32
StringsMatch(counted_string* S1, counted_string S2)
{
  b32 Result = StringsMatch(S1, &S2);
  return Result;
}

inline b32
StringsMatch(counted_string S1, counted_string S2)
{
  b32 Result = StringsMatch(&S1, &S2);
  return Result;
}

inline b32
Contains(const char *S1, const char *S2)
{
  const char *S1At = S1;
  while (*S1At)
  {
    const char *S2At = S2;

    b32 Result = (*S1At == *S2At);
    while (Result && *S1At && *S2At)
    {
      Result &= (*S1At++ == *S2At++);
    }

    if (Result && *S2At == 0)
    {
      return True;
    }
    else
    {
      ++S1At;
    }
  }

  return False;
}

bonsai_function counted_string
Trim(counted_string String)
{
  counted_string Result = String;
  while (Result.Count)
  {
    if (Result.Start[0] == ' ' || Result.Start[0] == '\n')
    {
      --Result.Count;
      ++Result.Start;
    }
    else
    {
      break;
    }
  }

  while (Result.Count)
  {
    if (Result.Start[Result.Count-1] == ' ' || Result.Start[Result.Count-1] == '\n')
    {
      --Result.Count;
    }
    else
    {
      break;
    }
  }

  return Result;
}

counted_string
Split(counted_string* String, char SplitTarget)
{
  counted_string Result = {};

  for (u32 CharIndex = 0;
      CharIndex < String->Count;
      ++CharIndex)
  {
    if (String->Start[CharIndex] == SplitTarget)
    {
      Result.Start = String->Start + CharIndex + 1;
      Result.Count = String->Count - CharIndex - 1;

      String->Count = CharIndex;
    }
  }

  return Result;
}

bonsai_function char
Advance(char_cursor* BufferCursor)
{
  char Result = 0;
  if (Remaining(BufferCursor))
  {
    Result = *BufferCursor->At++;
  }
  else
  {
    Error("Attempted to advance an empty char_cursor!");
  }
  return Result;
}

bonsai_function char
ToUpper(char C)
{
  u32 MapToUpper = 'a' - 'A';
  if (C >= 'a' && C <= 'z')
  {
    C -= MapToUpper;
  }
  return C;
}

bonsai_function b32
IsNumeric(char C)
{
  b32 Result = (C >= '0' && C <= '9');
  return Result;
}

bonsai_function u32
ToU32(char C)
{
  Assert(IsNumeric(C));
  u32 Result = (u32)C - (u32)'0';
  return Result;
}

bonsai_function u32
Exp(u32 Base, u32 Exponent)
{
  u32 Result = 1;
  for (u32 Ignored = 0;
      Ignored < Exponent;
      ++Ignored)
  {
    Result *= Base;
  }

  return Result;
}

bonsai_function u32
ToU32(counted_string S)
{
  u32 Result = 0;
  for (u32 CharIndex = 0;
      CharIndex < S.Count;
      ++CharIndex)
  {
    u32 Digit = ToU32(S.Start[CharIndex]);
    Result += (Digit * Exp(10, ((u32)S.Count - CharIndex - 1)));
  }

  return Result;
}

bonsai_function char
Peek(char_cursor* BufferCursor)
{
  char Result = *BufferCursor->At;
  return Result;
}

bonsai_function u32
EmbeddedU32(char_cursor* FormatCursor)
{
  char* NumStart = FormatCursor->At;
  Assert(IsNumeric(*NumStart));
  while(IsNumeric(Peek(FormatCursor))) { Advance(FormatCursor); }

  u32 CharCount = (u32)(FormatCursor->At - NumStart);
  counted_string NumberString = CS((const char*)NumStart, CharCount);
  u32 Result = ToU32(NumberString);
  return Result;
}

bonsai_function void
CopyToDest(char_cursor* Dest, char C)
{
  b32 DoCopy = True;
  if (!Remaining(Dest))
  {
    if (Dest->Memory)
    {
      umm CurrentSize = TotalSize(Dest);
      umm Increment = 32;
      if (Reallocate((u8*)Dest->Start, Dest->Memory, CurrentSize, CurrentSize+Increment))
      {
        Dest->End += Increment;
      }
      else
      {
        Error("Reallocation Failed");
        DoCopy = False;
      }
    }
    else
    {
      Error("No memory pointer!");
      DoCopy = False;
    }
  }

  if (DoCopy)
  {
    *Dest->At++ = C;
  }
}

bonsai_function void
u64ToChar(char_cursor* Dest, u64 Value, u32 Base = 10, char *Digits = DecChars)
{
  Assert(Base != 0);

  char *Start = Dest->At;
  do
  {
      u64 DigitIndex = (Value % Base);
      char Digit = Digits[DigitIndex];
      CopyToDest(Dest, Digit);
      Value /= Base;
  } while(Value != 0);

  char *End = Dest->At;

  while(Start < End)
  {
      --End;
      char Temp = *End;
      *End = *Start;
      *Start = Temp;
      ++Start;
  }

  return;
}

bonsai_function void
s64ToChar(char_cursor* Dest, s64 Value, u32 Base = 10, char *Digits = DecChars)
{
  if (Value < 0)
  {
    CopyToDest(Dest, '-');
    Value = -Value;
  }

  u64ToChar(Dest, (u64)Value, Base, Digits);
  return;
}

// Note(Jesse): Shamelessly copied, then modified, from the Handmade Hero codebase
#define DEFAULT_FORMAT_PRECISION (16)
bonsai_function void
f64ToChar(char_cursor* Dest, r64 Value, u32 Precision = DEFAULT_FORMAT_PRECISION)
{
  if(Value < 0)
  {
      CopyToDest(Dest, '-');
      Value = -Value;
  }

  u64 IntegerPart = (u64)Value;
  Value -= (r64)IntegerPart;
  u64ToChar(Dest, IntegerPart);

  CopyToDest(Dest, '.');

  // TODO(casey): Note that this is NOT an accurate way to do this!
  for(u32 PrecisionIndex = 0;
      PrecisionIndex < Precision;
      ++PrecisionIndex)
  {
      Value *= 10.0f;
      u32 Int = (u32)Value;
      Value -= (r32)Int;
      CopyToDest(Dest, DecChars[Int]);
  }

  return;
}

