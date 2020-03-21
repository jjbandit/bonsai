enum struct_member_type
{
  type_struct_member_noop,
  type_struct_member_variable,
  type_struct_member_function,
  type_struct_member_union,
};

struct struct_member
{
  struct_member_type Type;

  union
  {
    struct_member_variable struct_member_variable;
    struct_member_function struct_member_function;
    struct_member_union struct_member_union;
  };
};


