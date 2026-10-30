/* Additional N3886 diagnostic cases. */
int audit_nonempty;

/* N3886 6.5.4.5p1: sizeof requires a complete type. */
#ifdef TEST1
struct incomplete;
enum { invalid_size = sizeof(struct incomplete) }; /* ERROR */
#endif

/* N3886 6.5.4.4p1: unary + requires an arithmetic operand. */
#ifdef TEST2
struct aggregate { int member; } aggregate;
void test (void)
{
  +aggregate; /* ERROR */
}
#endif

/* N3886 6.5.4.4p1: unary ~ requires an integer operand. */
#ifdef TEST3
float floating;
void test (void)
{
  ~floating; /* ERROR */
}
#endif

/* N3886 6.5.4.4p1: unary ! requires a scalar operand. */
#ifdef TEST4
struct aggregate { int member; } aggregate;
void test (void)
{
  !aggregate; /* ERROR */
}
#endif

/* N3886 6.5.4.3p1: the address operator cannot take a bit-field address. */
#ifdef TEST5
struct bits { unsigned field: 3; } bits;
void test (void)
{
  &bits.field; /* ERROR */
}
#endif

/* N3886 6.5.4.3p2: indirection requires a pointer operand. */
#ifdef TEST6
int integer; /* IGNORE(GCC) */
void test (void)
{
  *integer; /* ERROR */
}
#endif

/* N3886 6.5.3.3p1: a called expression requires function pointer type. */
#ifdef TEST8
int integer; /* IGNORE(GCC) */
void test (void)
{
  integer(); /* ERROR */
}
#endif

/* Valid arithmetic, pointer indirection, array subscripting, and call. */
#ifdef TEST9
int array[3];
int function (void)
{
  return array[1];
}
int test (void)
{
  return +*array + function ();
}
#endif

/* N3886 6.5.3.3p2: a call supplies one argument per parameter. */
#ifdef TEST10
void function (int, int); /* IGNORE(GCC) */
void test (void)
{
  function (1); /* ERROR */
}
#endif

/* N3886 6.5.3.3p2: each argument is assignable to its parameter type. */
#ifdef TEST11
void function (int); /* IGNORE(GCC) */
struct aggregate { int member; } aggregate;
void test (void)
{
  function (aggregate); /* ERROR */
}
#endif

/* N3886 6.5.3.4p1: the left operand of . is a structure or union. */
#ifdef TEST12
int integer; /* IGNORE(GCC) */
void test (void)
{
  integer.member; /* ERROR */
}
#endif

/* N3886 6.5.3.4p1: the member named after . belongs to the type. */
#ifdef TEST13
struct aggregate { int member; } aggregate;
void test (void)
{
  aggregate.missing; /* ERROR */
}
#endif

/* N3886 6.5.3.5p1: postfix ++ requires a modifiable lvalue. */
#ifdef TEST15
const int value = 0; /* IGNORE(GCC) */
void test (void)
{
  value++; /* ERROR */
}
#endif

/* N3886 6.5.4.2p1: prefix ++ requires arithmetic or pointer type. */
#ifdef TEST16
struct aggregate { int member; } value;
void test (void)
{
  ++value; /* ERROR */
}
#endif

/* N3886 6.5.3.4p2: -> requires a pointer to a structure or union. */
#ifdef TEST18
int *pointer;
void test (void)
{
  pointer->member; /* ERROR */
}
#endif

/* N3886 6.5.3.4p2: the named member belongs to the pointed-to type. */
#ifdef TEST19
struct aggregate { int member; } *pointer;
void test (void)
{
  pointer->missing; /* ERROR */
}
#endif

/* N3886 6.5.3.2p2: [] requires an array or pointer operand. */
#ifdef TEST20
int integer; /* IGNORE(GCC) */
void test (void)
{
  integer[0]; /* ERROR */
}
#endif

/* Valid increments and pointer-to-structure member access. */
#ifdef TEST21
struct aggregate { int member; } value;
struct aggregate *pointer = &value;
void test (void)
{
  pointer->member++; ++pointer->member;
}
#endif
