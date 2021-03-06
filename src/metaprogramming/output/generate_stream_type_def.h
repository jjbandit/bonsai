
    
    struct type_def_stream_chunk
    {
      type_def Element;
      type_def_stream_chunk* Next;
    };

    
    struct type_def_stream
    {
      type_def_stream_chunk* FirstChunk;
      type_def_stream_chunk* LastChunk;
    };


    
    struct type_def_iterator
    {
      type_def_stream* Stream;
      type_def_stream_chunk* At;
    };

    bonsai_function type_def_iterator
    Iterator(type_def_stream* Stream)
    {
      type_def_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid(type_def_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance(type_def_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }


    
    bonsai_function type_def *
    Push(type_def_stream* Stream, type_def Element, memory_arena* Memory)
    {
      type_def_stream_chunk* NextChunk = (type_def_stream_chunk*)PushStruct(Memory, sizeof( type_def_stream_chunk ), 1, 0);
      NextChunk->Element = Element;

      if (!Stream->FirstChunk)
      {
        Assert(!Stream->LastChunk);
        Stream->FirstChunk = NextChunk;
        Stream->LastChunk = NextChunk;
      }
      else
      {
        Stream->LastChunk->Next = NextChunk;
        Stream->LastChunk = NextChunk;
      }

      Assert(NextChunk->Next == 0);
      Assert(Stream->LastChunk->Next == 0);

      type_def *Result = &NextChunk->Element;
      return Result;
    }

    bonsai_function void
    ConcatStreams( type_def_stream *S1, type_def_stream *S2)
    {
      if (S1->LastChunk)
      {
        Assert(S1->FirstChunk);

        if (S2->FirstChunk)
        {
          Assert(S2->LastChunk);
          S1->LastChunk->Next = S2->FirstChunk;
          S1->LastChunk = S2->LastChunk;
        }
        else
        {
          Assert(!S2->LastChunk);
        }
      }
      else
      {
        Assert(!S1->FirstChunk);
        Assert(!S1->LastChunk);

        if(S2->FirstChunk)
        {
          Assert(S2->LastChunk);
        }
        else
        {
          Assert(!S2->LastChunk);
        }

        *S1 = *S2;
      }
    }



