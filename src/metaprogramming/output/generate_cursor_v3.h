
    struct v3_cursor
    {
      v3* Start;
      v3* End;
      v3* At;
    };

    bonsai_function v3_cursor
    V3Cursor(umm ElementCount, memory_arena* Memory)
    {
      v3* Start = (v3*)PushStruct(Memory, sizeof( v3 ), 1, 0);
      v3_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

