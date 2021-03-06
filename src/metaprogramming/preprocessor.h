meta(
  func dunion_debug_print(DUnionType)
  {
    bonsai_function void
    DebugPrint( (DUnionType.type) UnionStruct, u32 Depth = 0)
    {
      switch(UnionStruct.Type)
      {
        (
          DUnionType.map_members (M)
          {
            (
              M.is_anonymous?
              {
                (
                  M.map_values (UnionMember)
                  {
                    case type_(UnionMember.type):
                    {
                      DebugPrint(UnionStruct.(UnionMember.type), Depth+4);
                    }
                  }
                )
              }
            )
          }
        )

        InvalidDefaultCase;
      }
    }
  }
)

meta(
  func buffer(Type)
  {
    struct (Type.name)_buffer
    {
      (Type.name)* Start;
      umm Count;
    };

    bonsai_function (Type.name)_buffer
    (Type.name.to_capital_case)Buffer(umm ElementCount, memory_arena* Memory)
    {
      (Type.name)_buffer Result = {};

      if (ElementCount)
      {
        Result.Start = Allocate( (Type.name), Memory, ElementCount );
        Result.Count = ElementCount;
      }
      else
      {
        Warn("Attempted to allocate (Type.name)_buffer of 0 length.");
      }

      return Result;
    }
  }
)

meta(
  func generate_cursor(Type)
  {
    struct (Type.name)_cursor
    {
      (Type.name)* Start;
      (Type.name)* At;
      (Type.name)* End;
    };

    bonsai_function (Type.name)_cursor
    (Type.name.to_capital_case)Cursor(umm ElementCount, memory_arena* Memory)
    {
      (Type.name)* Start = ((Type.name)*)PushStruct(Memory, sizeof( (Type.name) ), 1, 0);
      (Type.name)_cursor Result = {
        .Start = Start,
        .End = Start+ElementCount,
        .At = Start,
      };
      return Result;
    }
  }
)

meta(
  func generate_string_table(EnumType)
  {
    bonsai_function counted_string
    ToString( (EnumType.name) Type)
    {
      counted_string Result = {};
      switch (Type)
      {
        (
          EnumType.map_values (EnumValue)
          {
            case (EnumValue.name): { Result = CSz("(EnumValue.name)"); } break;
          }
        )
      }
      return Result;
    }
  }
)

meta(
  func generate_value_table(EnumType)
  {
    bonsai_function (EnumType.name)
    (EnumType.name.to_capital_case)(counted_string S)
    {
      (EnumType.name) Result = {};

      (
        EnumType.map_values(EnumValue)
        {
          if (StringsMatch(S, CSz("(EnumValue.name)"))) { return (EnumValue.name); }
        }
      )

      return Result;
    }
  }
)

meta(
  func generate_stream_chunk_struct(Type)
  {
    struct (Type.name)_stream_chunk
    {
      (Type.name) Element;
      (Type.name)_stream_chunk* Next;
    };
  }
)

meta(
  func generate_stream_push(Type)
  {
    bonsai_function (Type.name) *
    Push((Type.name)_stream* Stream, (Type.name) Element, memory_arena* Memory)
    {
      (Type.name)_stream_chunk* NextChunk = ((Type.name)_stream_chunk*)PushStruct(Memory, sizeof( (Type.name)_stream_chunk ), 1, 0);
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

      (Type.name) *Result = &NextChunk->Element;
      return Result;
    }

    bonsai_function void
    ConcatStreams( (Type.name)_stream *S1, (Type.name)_stream *S2)
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
  }
)

meta(
  func generate_stream_struct(Type)
  {
    struct (Type.name)_stream
    {
      (Type.name)_stream_chunk* FirstChunk;
      (Type.name)_stream_chunk* LastChunk;
    };

  }
)

meta(
  func generate_stream_iterator(Type)
  {
    struct (Type.name)_iterator
    {
      (Type.name)_stream* Stream;
      (Type.name)_stream_chunk* At;
    };

    bonsai_function (Type.name)_iterator
    Iterator((Type.name)_stream* Stream)
    {
      (Type.name)_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    bonsai_function b32
    IsValid((Type.name)_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    bonsai_function void
    Advance((Type.name)_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }

  }
)

meta(
  func generate_stream_getters(InputTypeDef)
  {
    InputTypeDef.map_members (Member) {
      bonsai_function InputTypeDef.type
      GetBy(Member.name)( (Member.Type) Needle, (InputTypeDef.type)_stream *Haystack)
      {
        // TODO : Implement matching!
      }
    }
  }
)

meta(
  func generate_stream(Type)
  {
    (generate_stream_chunk_struct(Type))
    (generate_stream_struct(Type))
    (generate_stream_iterator(Type))
    (generate_stream_push(Type))

  }
)

meta(
  func string_and_value_tables(Def)
  {
    ( generate_string_table(Def) )
    ( generate_value_table(Def) )
  }
)

meta(
  func stream_and_cursor(Def)
  {
    ( generate_cursor(Def) )
    ( generate_stream(Def) )
  }
)

meta(
  polymorphic_func void DebugPrint( @TypeDef RuntimeValue, u32 Depth)
  {
    DebugPrint("(TypeDef.name): ", Depth);
    TypeDef.is_enum?
    {
      TypeDef.map_values (ValueDef)
      {
        DebugPrint("(ValueDef.name) (ValueDef.value)", Depth+1);
        DebugPrint(ToString(RuntimeValue), Depth+1);
      }
    }

    TypeDef.is_struct?
    {
      TypeDef.map_members (MemberDef)
      {
        DebugPrint("(MemberDef.type) (MemberDef.name): ", Depth);
        DebugPrint(RuntimeValue.(MemberDef.name), Depth+1);
        DebugPrint("\n");
      }
    }
  }
)







enum d_union_flags
{
  d_union_flag_none,
  d_union_flag_enum_only
};

enum metaprogramming_directive
{
  meta_directive_noop,
  enum_only,
  d_union,
  named_list,
  for_datatypes,

  func,
  polymorphic_func,
};
meta( string_and_value_tables(metaprogramming_directive) )
#include <metaprogramming/output/string_and_value_tables_metaprogramming_directive.h>

enum meta_arg_operator
{
  meta_arg_operator_noop,

  name,
  type,
  value,
  map_values,
  map_members,
  is_enum,
  is_struct,
};
meta( generate_value_table(meta_arg_operator) )
#include <metaprogramming/output/generate_value_table_meta_arg_operator.h>

enum meta_transform_op
{
  meta_transform_op_noop,

  to_capital_case = (1 << 0),
};
meta(generate_value_table(meta_transform_op))
#include <metaprogramming/output/generate_value_table_meta_transform_op.h>






enum c_token_type
{
  CTokenType_Unknown = 0,

  CTokenType_OpenBracket   = '[',
  CTokenType_CloseBracket  = ']',
  CTokenType_OpenBrace     = '{',
  CTokenType_CloseBrace    = '}',
  CTokenType_OpenParen     = '(',
  CTokenType_CloseParen    = ')',
  CTokenType_Dot           = '.',
  CTokenType_Comma         = ',',
  CTokenType_Semicolon     = ';',
  CTokenType_Colon         = ':',
  CTokenType_Hash          = '#',
  CTokenType_At            = '@',
  CTokenType_Dollar        = '$',
  CTokenType_Space         = ' ',
  CTokenType_Tab           = '\t',
  CTokenType_Star          = '*',
  CTokenType_Ampersand     = '&', // TODO(Jesse id: 238, tags: immediate, cleanup): Change name to BitwiseAnd
  CTokenType_SingleQuote   = '\'',
  CTokenType_DoubleQuote   = '"',
  CTokenType_Equals        = '=',
  CTokenType_LT            = '<',
  CTokenType_GT            = '>',
  CTokenType_Plus          = '+',
  CTokenType_Minus         = '-',
  CTokenType_Percent       = '%',
  CTokenType_Bang          = '!',
  CTokenType_Hat           = '^',
  CTokenType_Question      = '?',
  CTokenType_FSlash        = '/',
  CTokenType_BSlash        = '\\',
  CTokenType_Tilde         = '~',
  CTokenType_Backtick      = '`',
  CTokenType_Pipe          = '|', // TODO(Jesse id: 239, tags: immediate, cleanup): Change name to BitwiseOr
  CTokenType_Newline       = '\n',
  CTokenType_CarrigeReturn = '\r',
  CTokenType_EOF           = EOF,


  CTokenType_CommentSingleLine     = 256, // Making the assumption we'll never parse anything that's not ascii, so start the non-ascii tokens at 256
  CTokenType_CommentMultiLineStart,
  CTokenType_CommentMultiLineEnd,

  CTokenType_EscapedNewline,

  CTokenType_Identifier,

  CTokenType_StringLiteral,
  CTokenType_CharLiteral,
  CTokenType_IntLiteral,
  CTokenType_DoubleLiteral,
  CTokenType_FloatLiteral,

  CTokenType_Meta,

  CTokenType_Struct,
  CTokenType_Enum,
  CTokenType_Union,
  CTokenType_Typedef,

  CTokenType_Using,

  CTokenType_ThreadLocal,
  CTokenType_Const,
  CTokenType_Static,
  CTokenType_Volatile,
  CTokenType_Long,
  CTokenType_Unsigned,
  CTokenType_Signed,

  CTokenType_Bool,
  CTokenType_M128,
  CTokenType_Auto,
  CTokenType_Void,
  CTokenType_Double,
  CTokenType_Float,
  CTokenType_Char,
  CTokenType_Int,
  CTokenType_Short,

  CTokenType_Inline,
  CTokenType_TemplateKeyword,
  CTokenType_OperatorKeyword,

  CTokenType_Extern,
  CTokenType_Asm,
  CTokenType_Goto,

  CTokenType_Ellipsis,

  CTokenType_If,
  CTokenType_Else,
  CTokenType_Break,
  CTokenType_Switch,
  CTokenType_Case,
  CTokenType_Default,
  CTokenType_For,
  CTokenType_While,
  CTokenType_Continue,
  CTokenType_Return,

  CTokenType_LeftShift,
  CTokenType_RightShift,

  CTokenType_LessEqual,
  CTokenType_GreaterEqual,
  CTokenType_AreEqual,
  CTokenType_NotEqual,

  CTokenType_PlusEquals,
  CTokenType_MinusEquals,
  CTokenType_TimesEquals,
  CTokenType_DivEquals,
  CTokenType_ModEquals,
  CTokenType_AndEquals,
  CTokenType_OrEquals,
  CTokenType_XorEquals,

  CTokenType_Increment,
  CTokenType_Decrement,
  CTokenType_LogicalAnd,
  CTokenType_LogicalOr,

  CTokenType_Arrow,

  CT_Pragma,

  CT_ScopeResolutionOperator,
  CT_NameQualifier,

  CT_MacroLiteral,

  CT_PreprocessorPaste,

  CT_PreprocessorInclude,
  CT_PreprocessorIf,
  CT_PreprocessorElse,
  CT_PreprocessorElif,
  CT_PreprocessorEndif,
  CT_PreprocessorIfDefined,
  CT_PreprocessorIfNotDefined,
  CT_PreprocessorDefine,
  CT_PreprocessorUndef,
  CT_PreprocessorPragma,
  CT_PreprocessorError,
  CT_PreprocessorWarning,
  CT_Preprocessor__VA_ARGS__,
};
meta(generate_string_table(c_token_type))
#include <metaprogramming/output/generate_string_table_c_token_type.h>

struct macro_def;
struct c_token
{
  c_token_type Type;
  counted_string Value;
  b32 Erased;

  union {
    /* s64 SignedValue; */ // TODO(Jesse id: 272): Fold `-` sign into this value at tokenization time?
    u64 UnsignedValue;
    r64 FloatValue;
    macro_def *Macro;
    c_token *QualifierName;
  };

};
meta(generate_cursor(c_token))
#include <metaprogramming/output/generate_cursor_c_token.h>

meta(buffer(c_token))
#include <metaprogramming/output/buffer_c_token.h>

meta(buffer(c_token_buffer))
#include <metaprogramming/output/buffer_c_token_buffer.h>

meta(generate_stream(c_token_buffer))
#include <metaprogramming/output/generate_stream_c_token_buffer.h>

struct parser
{
  b32 Valid = 1;
  c_token_cursor Tokens;

  /* TODO(Jesse id: 154) This is pretty shitty because whenever we copy one of
   * these structs this field has to be manually zeroed out ..
   */
  c_token_cursor OutputTokens;

  counted_string Filename;
  u32 LineNumber;

  parser *Prev;
  parser *Next;
};
meta(generate_cursor(parser))
#include <metaprogramming/output/generate_cursor_parser.h>

meta(generate_stream(parser))
#include <metaprogramming/output/generate_stream_parser.h>



meta(
  d_union struct_member_function
  {
    struct_member_function_normal      enum_only
    struct_member_function_operator    enum_only
    struct_member_function_constructor enum_only
    struct_member_function_destructor  enum_only
  }
)
#include <metaprogramming/output/d_union_c_decl_function.h>

struct struct_member_stream_chunk;

/* TODO(Jesse id: 191, tags: metaprogramming): This can be generated, but it
 * requires lazily evaluating functions once their dependant types have been
 * generated.  Not sure if this is a good idea or not, but we could do it.
 *
 * meta( generate_stream_struct(struct_member) )
 *
 * Currently the invariant of being able to remove and re-generate all meta
 * output in a single run of the metaprogramming system is broken by this.
 */

struct struct_member_stream
{
  struct_member_stream_chunk *FirstChunk;
  struct_member_stream_chunk *LastChunk;
};

struct struct_def
{
  counted_string Type;
  counted_string DefinedInFile;
  struct_member_stream Members;

  b32 IsUnion;
};
meta(stream_and_cursor(struct_def))
#include <metaprogramming/output/stream_and_cursor_struct_def.h>

struct struct_member_anonymous
{
  struct_def Body;
};

// Note(Jesse): These are just placeholder names such that we can do correct
// type resolution when we see STL containers.
struct stl_container_def
{
  counted_string Name;
};
meta(generate_stream(stl_container_def))
#include <metaprogramming/output/generate_stream_stl_container_def.h>

enum datatype_type
{
  type_datatype_noop,

  type_struct_def,
  type_struct_member,

  type_enum_def,
  type_enum_member,

  type_type_def,

  type_primitive_def,

  type_stl_container_def,
};

struct struct_member;
struct struct_def;
struct enum_def;
struct enum_member;
struct type_def;
/* TODO(Jesse, id: 188, tags: cleanup) This should have the name property,
 * instead of having the struct and enum defs have seperate names.
 *
 * Actually .. it's unclear if this is true or not anymore since we've added
 * a bunch more stuff to the union.
 */
struct datatype
{
  datatype_type Type;

  union
  {
    struct_def         *struct_def;
    struct_member      *struct_member;

    enum_def           *enum_def;
    enum_member        *enum_member;

    type_def           *type_def;

    stl_container_def  *stl_container_def;
  };
};

enum linkage_type
{
  linkage_noop,
  linkage_extern,
  linkage_extern_c,
};

struct type_spec
{
  c_token Token;

  datatype Datatype;

  u32 ReferenceLevel;
  u32 IndirectionLevel;

  b32 IsMetaTemplateVar;

  b32 HasTemplateArguments;

  b32 ThreadLocal;
  b32 Const;
  b32 Static;
  b32 Volatile;

  b32 Unsigned;
  b32 Signed;

  b32 Long;
  b32 LongLong;
  b32 Short;

  b32 Struct;
  b32 Enum;

  b32 Inline;
  b32 IsFunctionPointer;
  counted_string FunctionPointerTypeName;

  linkage_type Linkage;

  counted_string TemplateSource;
  counted_string SourceText;
};

struct ast_node;
struct variable_decl
{
  type_spec Type;
  counted_string Name;
  ast_node *StaticBufferSize;
  ast_node *Value;
};
meta(generate_stream(variable_decl))
#include <metaprogramming/output/generate_stream_variable_decl.h>

enum function_type
{
  function_type_noop,
  function_type_constructor,
  function_type_destructor,
  function_type_operator,
  function_type_normal,
};

struct ast_node_statement;
struct function_decl
{
  function_type Type;

  counted_string Name;
  type_spec ReturnType;

  variable_decl_stream Args;

  b32 IsVariadic;

  parser Body;
  ast_node_statement *Ast;
};

meta(generate_stream(function_decl))
#include <metaprogramming/output/generate_stream_function_decl.h>

/* TODO(Jesse, id: 290, tags: metaprogramming, improvement): generating this:
 * meta( d_union declaration { function_decl variable_decl })
 * results in a name collision with the struct_member union tag.
 *
 * Should we have some way of overriding the tag name it generates?  Or
 * potentially modify the way we generate type tags such that name collisions
 * won't happen.  I'd prefer an override to keep the tag names as concise as
 * possible, but maybe once the preprocessor generates the switch statements
 * for us it won't matter if they're overly verbose.
 */
enum declaration_type
{
  type_declaration_noop,
  type_declaration_function_decl,
  type_declaration_variable_decl,
};

struct declaration
{
  declaration_type Type;

  union
  {
    function_decl function_decl;
    variable_decl variable_decl;
  };
};


meta(
  d_union struct_member
  {
    variable_decl
    function_decl
    struct_member_anonymous
  }
)
#include <metaprogramming/output/d_union_c_decl.h>

meta(generate_cursor(struct_member))
#include <metaprogramming/output/generate_cursor_c_decl.h>

meta( generate_stream_chunk_struct(struct_member) )
#include <metaprogramming/output/generate_stream_chunk_struct_c_decl.h>

struct ast_node_expression;
struct enum_member
{
  counted_string Name;
  ast_node_expression *Value;
};
meta(generate_stream(enum_member))
#include <metaprogramming/output/generate_stream_enum_field.h>

struct d_union_member
{
  counted_string Type;
  counted_string Name;
  d_union_flags Flags;
};
meta(generate_stream(d_union_member))
#include <metaprogramming/output/generate_stream_d_union_member.h>

struct enum_def
{
  counted_string Name;
  enum_member_stream Members;
};
meta(stream_and_cursor(enum_def))
#include <metaprogramming/output/stream_and_cursor_enum_def.h>

struct type_def
{
  type_spec Type;
  counted_string Alias;
};
meta(generate_stream(type_def))
#include <metaprogramming/output/generate_stream_type_def.h>


struct primitive_def
{
  type_spec Type;
};
meta(generate_stream(primitive_def))
#include <metaprogramming/output/generate_stream_primitive_def.h>


bonsai_function datatype
Datatype(struct_member* M)
{
  datatype Result = {
    .Type = type_struct_member,
    .struct_member = M,
  };
  return Result;
}

bonsai_function datatype
Datatype(enum_member* E)
{
  datatype Result = {
    .Type = type_enum_member,
    .enum_member = E,
  };
  return Result;
}


bonsai_function datatype
Datatype(struct_def* S)
{
  datatype Result = {
    .Type = type_struct_def,
    .struct_def = S,
  };
  return Result;
}

bonsai_function datatype
Datatype(type_def* E)
{
  datatype Result = {
    .Type = type_type_def,
    .type_def = E,
  };
  return Result;
}

bonsai_function datatype
Datatype(stl_container_def* E)
{
  datatype Result = {
    .Type = type_stl_container_def,
    .stl_container_def = E,
  };
  return Result;
}

bonsai_function datatype
Datatype(enum_def* E)
{
  datatype Result = {
    .Type = type_enum_def,
    .enum_def = E,
  };
  return Result;
}

struct meta_func_arg
{
  counted_string Match;
  datatype Data;
};
meta(generate_stream(meta_func_arg))
#include <metaprogramming/output/generate_stream_meta_func_arg.h>










struct d_union_decl
{
  counted_string Name;

  d_union_member_stream Members;
  struct_member_stream CommonMembers;

  counted_string CustomEnumType;
};

enum macro_type
{
  type_macro_noop,

  type_macro_keyword,
  type_macro_function,
};

struct macro_def
{
  macro_type Type;
  counted_string Name;
  parser Body;

  counted_string_buffer NamedArguments;
  b32 Variadic;
  b32 Undefed; // Gets toggled when we hit an undef
};
meta(generate_stream(macro_def))
#include <metaprogramming/output/generate_stream_macro_def.h>





struct meta_func
{
  counted_string Name;
  counted_string ArgName;
  parser Body;
};
meta(generate_stream(meta_func))
#include <metaprogramming/output/generate_stream_meta_func.h>


struct todo
{
  counted_string Id;
  counted_string Value;
  b32 FoundInCodebase;
};
meta(generate_stream(todo))
#include <metaprogramming/output/generate_stream_todo.h>

struct tag
{
  counted_string Tag;
  todo_stream Todos;
};
meta(generate_stream(tag))
#include <metaprogramming/output/generate_stream_tag.h>

struct person
{
  counted_string Name;
  tag_stream Tags;
};
meta(generate_stream(person))
#include <metaprogramming/output/generate_stream_person.h>

#define SafeAccess(T, Ptr) (&(Ptr)->T); Assert((Ptr)->Type == type_##T)

struct ast_node_expression
{
  ast_node *Value;
  ast_node_expression *Next;
};
meta(generate_stream(ast_node_expression))
#include <metaprogramming/output/generate_stream_ast_node_expression.h>

struct ast_node_statement
{
  ast_node_expression *LHS;
  ast_node_statement  *RHS;
  ast_node_statement  *Next;
};

struct ast_node_function_call
{
  counted_string Name;
  function_decl *Prototype;
  ast_node_expression_stream Args;
};

struct ast_node_type_specifier
{
  datatype Datatype;
  type_spec TypeSpec;
  ast_node_expression *Name;
};

struct ast_node_variable_def
{
  datatype Type;
  variable_decl Decl;
  ast_node *Value;
};
meta(generate_stream(ast_node_variable_def))
#include <metaprogramming/output/generate_stream_ast_node_variable_def.h>

struct ast_node_access
{
  c_token AccessType; // Arrow or Dot
  ast_node_expression *Symbol;
};

struct ast_node_parenthesized
{
  ast_node_expression *Expr;

  // TODO(Jesse id: 262): These should probably be in their own type_ast_node_cast .. ?
  b32 IsCast;
  ast_node_expression *CastValue;
};

struct ast_node_initializer_list
{
  u32 Ignored;
};

struct ast_node_operator
{
  ast_node_expression *Operand;
  c_token Token;
};

struct ast_node_literal
{
  c_token Token;
};

struct ast_node_symbol
{
  c_token Token;
};

struct ast_node_return
{
  ast_node_expression *Value;
};

struct ast_node_predicated
{
  c_token Token;
  ast_node_expression *Predicate;
};

meta(
  d_union ast_node
  {
    ast_node_statement
    ast_node_access
    ast_node_literal
    ast_node_expression
    ast_node_parenthesized
    ast_node_operator
    ast_node_function_call
    ast_node_return
    ast_node_initializer_list
    ast_node_symbol
    ast_node_variable_def
    ast_node_type_specifier
  }
)
#include <metaprogramming/output/d_union_ast_node.h>

meta(generate_stream(ast_node))
#include <metaprogramming/output/generate_stream_ast_node.h>

bonsai_function ast_node*
AllocateAstNode(ast_node_type T, ast_node **Result, memory_arena* Memory)
{
  Assert(Result && (*Result == 0)); // We got a valid pointer, and it hasn't been allocated yet.

  *Result = AllocateProtection(ast_node, Memory, 1, False);
  (*Result)->Type = T;

  return *Result;
}

#define AllocateAndCastTo(T, NodeDest, Memory) (&AllocateAstNode(type_##T, (NodeDest), (Memory))->T)

struct arguments
{
  counted_string Outpath;
  counted_string_cursor Files;

  counted_string_cursor IncludePaths;

  b32 DoDebugWindow;
};

struct program_datatypes
{
  struct_def_stream        Structs;
  enum_def_stream          Enums;
  function_decl_stream     Functions;
  type_def_stream          Typedefs;
  macro_def_stream         Macros;
  primitive_def_stream     Primitives;
  stl_container_def_stream StlContainers;
};

struct for_enum_constraints
{
  // Replacement Patterns
  counted_string TypeName;
  counted_string ValueName;
};

struct body_text_constraints
{
  counted_string MemberContains;

  // Replacement Patterns
  counted_string TypeTag;
  counted_string TypeName;
  counted_string ValueName;
};

struct todo_list_info
{
  person_stream People;
  tagged_counted_string_stream_stream NameLists;
};



struct string_from_parser
{
  parser *Parser;
  const char* Start;
};

enum output_mode
{
  Output_NoOverwrite,
  Output_Unsafe,
};




inline void
PrintToken(c_token *Token)
{
  if (Token)
  {
    Assert(Token->Type);
    Assert(Token->Value.Start && Token->Value.Count);
    const char* Color = 0;

    switch (Token->Type)
    {
      case CT_MacroLiteral:
      {
        Color = BLUE_TERMINAL;
        if (Token->Erased)
        {
          Color = PURPLE_TERMINAL;
        }
      } break;

      default:
      {
        if (Token->Erased)
        {
          Color = RED_TERMINAL;
        }
      } break;
    }


    if (Color)
    {
      Log(Color);
    }

    Log("%S", Token->Value);

    if (Color)
    {
      Log(WHITE_TERMINAL);
    }
  }
}

inline void
PrintToken(c_token Token)
{
  PrintToken(&Token);
}

// TODO(Jesse, id: 347, tags:immediate) : Nuke this
b32
operator==(c_token T1, c_token T2)
{
  b32 Result = (T1.Type == T2.Type);
  if (Result && (T1.Value.Count > 1 || T2.Value.Count > 1) )
  {
    Result &= StringsMatch(T1.Value, T2.Value);
  }
  return Result;
}

b32
operator!=(c_token T1, c_token T2)
{
  b32 Result = !(T1==T2);
  return Result;
}

inline c_token
CToken(r32 FloatValue)
{
  c_token Result = {
    .Type = CTokenType_Float,
    .Value = FormatCountedString(TranArena, CSz("%f"), FloatValue), // TODO(Jesse id: 350, tags: memory_leak)
    .FloatValue = FloatValue
  };
  return Result;
}

inline c_token
CToken(u32 UnsignedValue)
{
  c_token Result = {
    .Type = CTokenType_IntLiteral,
    .Value = FormatCountedString(TranArena, CSz("%u"), UnsignedValue), // TODO(Jesse id: 351, tags: memory_leak)
    .UnsignedValue = UnsignedValue,
  };
  return Result;
}

inline c_token
CToken(counted_string Value)
{
  c_token Result = {
    .Type = CTokenType_Identifier,
    .Value = Value
  };
  return Result;
}

inline c_token
CToken(c_token_type Type, counted_string Value = CSz(""))
{
  c_token Result = {
    .Type = Type,
    .Value = Value
  };

  return Result;
}

c_token_cursor
AllocateTokenBuffer(memory_arena* Memory, u32 Count)
{
  c_token_cursor Result = {};
  Result.Start = AllocateProtection(c_token, Memory, Count, False);
  Result.At = Result.Start;
  Result.End = Result.Start + Count;

  return Result;
}

// TODO(Jesse): Rewrite one of these in terms of the other.  There's also code
// in the Push() function for the parser_stream type that might be useful to
// look at.
//
// @parser_allocation_duplication
bonsai_function parser*
AllocateParserPtr(counted_string Filename, u32 TokenCount, u32 OutputBufferTokenCount, memory_arena *Memory)
{
  parser *Result = AllocateProtection(parser, Memory, 1, False);

  Result->Filename = Filename;

  Result->Tokens = AllocateTokenBuffer(Memory, TokenCount);
  if (!Result->Tokens.Start)
  {
    Error("Allocating Token Buffer");
    return Result;
  }

  if (OutputBufferTokenCount)
  {
    Result->OutputTokens = AllocateTokenBuffer(Memory, OutputBufferTokenCount);
    if (!Result->OutputTokens.Start)
    {
      Error("Allocating OutputTokens Buffer");
      return Result;
    }
  }

  return Result;
}

// @parser_allocation_duplication
bonsai_function parser
AllocateParser(counted_string Filename, u32 TokenCount, u32 OutputBufferTokenCount, memory_arena *Memory)
{
  parser Result = {
    .Filename = Filename
  };

  Result.Tokens = AllocateTokenBuffer(Memory, TokenCount);
  if (!Result.Tokens.Start)
  {
    Error("Allocating Token Buffer");
    return Result;
  }

  if (OutputBufferTokenCount)
  {
    Result.OutputTokens = AllocateTokenBuffer(Memory, OutputBufferTokenCount);
    if (!Result.OutputTokens.Start)
    {
      Error("Allocating OutputTokens Buffer");
      return Result;
    }
  }

  return Result;
}

c_token
PeekToken(ansi_stream* Stream, u32 Lookahead = 0)
{
  c_token Result = {};

  if (Stream->At+Lookahead < Stream->End)
  {
    char At = *(Stream->At+Lookahead);
    // TODO(Jesse, id: 193, tags: metaprogramming): Metaprogram this.  I've had bugs multiple times because of it.
    switch (At)
    {
      case CTokenType_OpenBracket:
      case CTokenType_CloseBracket:
      case CTokenType_OpenBrace:
      case CTokenType_CloseBrace:
      case CTokenType_OpenParen:
      case CTokenType_CloseParen:
      case CTokenType_Dot:
      case CTokenType_Comma:
      case CTokenType_Semicolon:
      case CTokenType_Colon:
      case CTokenType_Hash:
      case CTokenType_At:
      case CTokenType_Dollar:
      case CTokenType_Space:
      case CTokenType_Tab:
      case CTokenType_Star:
      case CTokenType_Ampersand:
      case CTokenType_SingleQuote:
      case CTokenType_DoubleQuote:
      case CTokenType_Equals:
      case CTokenType_LT:
      case CTokenType_GT:
      case CTokenType_Plus:
      case CTokenType_Minus:
      case CTokenType_Percent:
      case CTokenType_Bang:
      case CTokenType_Hat:
      case CTokenType_Question:
      case CTokenType_FSlash:
      case CTokenType_BSlash:
      case CTokenType_Tilde:
      case CTokenType_Backtick:
      case CTokenType_Pipe:
      case CTokenType_Newline:
      case CTokenType_CarrigeReturn:
      case CTokenType_EOF:
      {
        Result = { .Type = (c_token_type)At };
      } break;
    }
  }
  else
  {
    Warn("Attempted to get token past end of stream on file : %.*s", (u32)Stream->Filename.Count, Stream->Filename.Start);
  }

  return Result;
}

meta(generate_stream_iterator(struct_member))
#include <metaprogramming/output/generate_stream_iterator_c_decl.h>

meta(generate_stream_push(struct_member))
#include <metaprogramming/output/generate_stream_push_c_decl.h>

bonsai_function string_from_parser
StartStringFromParser(parser* Parser)
{
  // NOTE(Jesse): This is completely broken if the string spans a parser chain link

  string_from_parser Result = {
    .Parser = Parser,
    .Start = Parser->Tokens.At->Value.Start
  };
  return Result;
}

bonsai_function counted_string
FinalizeStringFromParser(string_from_parser* Builder)
{
  // NOTE(Jesse): This is completely broken if the string spans a parser chain link
  //
  // TODO(Jesse, tags: immediate): Detect the above case and allocate memory?
  // Or.. maybe don't use the string_from_parser thing anymore?  On a cursory
  // inspection of the codebase we could potentially make it an error to have
  // that case occur.  I didn't find anywhere that case is ever defined to be
  // valid.  IE. having an include path span a macro expansion or another
  // include statement isn't a well defined program.

  umm Count = (umm)(Builder->Parser->Tokens.At->Value.Start - Builder->Start);
  counted_string Result = CS(Builder->Start, Count);
  return Result;
}

bonsai_function c_token_type
CloseTokenFor(c_token_type T)
{
  c_token_type Result = {};

  switch (T)
  {
    case CTokenType_OpenParen:
      Result = CTokenType_CloseParen; break;

    case CTokenType_OpenBrace:
      Result = CTokenType_CloseBrace; break;

    case CTokenType_OpenBracket:
      Result = CTokenType_CloseBracket; break;

    InvalidDefaultCase;
  }

  return Result;
}

enum parser_push_type
{
  parser_push_type_noop,

  parser_push_type_root,
  parser_push_type_include,
  parser_push_type_macro,
};

struct parse_context
{
  parser                *CurrentParser;

  program_datatypes      Datatypes;
  parser_stream          AllParsers;
  counted_string_cursor *IncludePaths;
  meta_func_stream       MetaFunctions;
  memory_arena          *Memory;
};
