/* auto infers the initialiser type after value conversions. */
#ifdef __SDCC
#pragma std_c23
#endif

/* An inferred copy of a const scalar must be modifiable. */
#ifdef TEST1
int f(void)
{
  const int source = 1;
  auto copy = source;
  ++copy;
  return copy;
}
#endif

/* Lvalue conversion removes volatile from the inferred value type. */
#ifdef TEST2
void f(void)
{
  volatile int source = 1;
  auto copy = source;
  _Static_assert(_Generic(&copy, int *: 1, default: 0), "unqualified copy");
}
#endif

/* A const pointer object must produce a modifiable pointer copy. */
#ifdef TEST3
void f(int *source)
{
  int *const original = source;
  auto copy = original;
  ++copy;
  (void)copy;
}
#endif

/* An array initialiser must undergo array-to-pointer conversion. */
#ifdef TEST4
void f(void)
{
  int array[2];
  auto pointer = array;
  pointer[0] = 1;
}
#endif

/* String inference retains the existing non-const-pointer warning. */
#ifdef TEST5
void f(void)
{
  auto pointer = "abc"; /* WARNING */
  ++pointer;
  (void)pointer;
}
#endif

/* A function designator must produce a callable function pointer. */
#ifdef TEST6
int g(int)
/* PDK14 requires reentrancy for an indirect call with an int argument. */
#if defined(__SDCC_pdk13) || defined (__SDCC_pdk14) || defined(__SDCC_pdk15)
__reentrant
#endif
;
int f(void)
{
  auto pointer = g;
  return pointer(1);
}
#endif

/* A scalar literal must infer a modifiable object. */
#ifdef TEST7
int f(void)
{
  auto value = 1;
  ++value;
  return value;
}
#endif

/* A qualifier written on the inferred declaration must be retained. */
#ifdef TEST8
void f(void)
{
  auto const value = 1;
  ++value; /* ERROR */
}
#endif

/* Array conversion must preserve const on the element type. */
#ifdef TEST9
void f(void)
{
  const int array[2] = {1, 2};
  auto pointer = array;
  pointer[0] = 3; /* ERROR */
}
#endif

/* Lvalue conversion must preserve qualifiers on the pointed-to type. */
#ifdef TEST10
void f(const int *source)
{
  auto pointer = source;
  *pointer = 2; /* ERROR */
}
#endif

/* A const aggregate must produce a modifiable aggregate copy. */
#ifdef TEST11
struct S
{
  int member;
};
int f(void)
{
  const struct S source = {1};
  auto copy = source;
  copy.member = 2;
  return copy.member;
}
#endif

/* Only the outer array dimension decays to a pointer. */
#ifdef TEST12
int f(void)
{
  int array[2][3];
  auto pointer = array;
  pointer[1][2] = 7;
  return pointer[1][2];
}
#endif

/* Lvalue conversion does not apply integer promotions. */
#ifdef TEST13
int f(void)
{
  unsigned char source = 1;
  auto copy = source;
  _Static_assert(sizeof(copy) == sizeof(source), "no integer promotion");
  return copy;
}
#endif

/* Inspecting a function address must leave its initialiser usable. */
#ifdef TEST14
int g(int)
/* PDK14 requires reentrancy for an indirect call with an int argument. */
#if defined(__SDCC_pdk13) || defined (__SDCC_pdk14) || defined(__SDCC_pdk15)
__reentrant
#endif
;
int f(void)
{
  auto pointer = &g;
  return pointer(1);
}
#endif

/* A volatile qualifier on the new declaration must be retained. */
#ifdef TEST15
void f(void)
{
  int source = 1;
  auto volatile copy = source;
  _Static_assert(_Generic(&copy, volatile int *: 1, default: 0), "explicit volatile");
}
#endif
