
    struct c_token_cursor
    {
      c_token* Start;
      c_token* End;
      c_token* At;
    };

    bonsai_function c_token_cursor
    CTokenCursor(umm ElementCount, memory_arena* Memory)
    {
      c_token* Start = (c_token*)PushStruct(Memory, sizeof( c_token ), 1, 0);
      c_token_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }

