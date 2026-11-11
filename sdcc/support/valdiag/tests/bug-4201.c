/* Tags in one scope must consistently denote a struct, union or enum. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

void unrelated (void);

#ifdef TEST1
/* A complete structure cannot be redefined as a union. */
struct S { int x; };
union S { int x; }; /* ERROR */
#endif

#ifdef TEST2
/* Check the reverse order too. */
union S { int x; };
struct S { int x; }; /* ERROR */
#endif

#ifdef TEST3
/* Enum tags share the tag namespace with structure tags. */
struct S { int x; };
enum S { value }; /* ERROR */
#endif

#ifdef TEST4
enum S { value };
struct S { int x; }; /* ERROR */
#endif

#ifdef TEST5
/* Enum tags also share the namespace with union tags. */
union S { int x; };
enum S { value }; /* ERROR */
#endif

#ifdef TEST6
enum S { value };
union S { int x; }; /* ERROR */
#endif

#ifdef TEST7
/* A forward declaration already establishes the tag kind. */
struct S;
union S { int x; }; /* ERROR */
#endif

#ifdef TEST8
/* Distinct inner-scope definitions may use another tag kind. */
struct S { int x; };
int test (void)
{
  union S { int x; };
  union S s = {3};
  return s.x;
}
#endif

#ifdef TEST9
enum S { outer_value };
int test (void)
{
  struct S { int x; };
  struct S s = {3};
  return s.x;
}
#endif

#ifdef TEST10
struct S { int x; };
int test (void)
{
  enum S { inner_value = 3 };
  return inner_value;
}
#endif

#ifdef TEST11
/* Completing a forward declaration with its original kind remains valid. */
struct S;
struct S { int x; };
struct S s;
#endif

#ifdef TEST12
/* C23 permits matching repeated definitions of the same tagged type. */
#ifndef SKIP_C23
struct S { int x; };
struct S { int x; };
union U { int x; };
union U { int x; };
#endif
#endif

#ifdef TEST13
#ifndef SKIP_C23
/* A fixed underlying type does not create a separate tag namespace. */
struct S { int x; };
enum S : int { value }; /* ERROR(SDCC) */
#endif
#endif

#ifdef TEST14
#ifndef SKIP_C23
enum S : int { value };
union S { int x; }; /* ERROR(SDCC) */
#endif
#endif
