/* Assignment requires an aggregate with no const-qualified member. */
void unrelated (void);

#ifdef TEST1
/* A const scalar member prevents structure assignment. */
struct S { const int x; };
void test (struct S *a, struct S *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST2
/* The restriction includes members of nested structures. */
struct I { const int x; };
struct S { struct I i; };
void test (struct S *a, struct S *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST3
/* A union also contains its const-qualified members. */
union U { const int x; int y; };
void test (union U *a, union U *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST4
/* Array element qualification contributes to aggregate constness. */
struct S { const int x[2]; };
void test (struct S *a, struct S *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST5
/* The pointer member itself is const. */
struct S { int *const p; };
void test (struct S *a, struct S *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST6
/* Recursion includes aggregate elements of array members. */
struct I { const int x; };
struct S { struct I a[2]; };
void test (struct S *a, struct S *b)
{
  *a = *b; /* ERROR */
}
#endif

#ifdef TEST7
/* Pointing to const does not make the pointer member const. */
struct S { const int *p; };
void test (struct S *a, struct S *b)
{
  *a = *b;
}
#endif

#ifdef TEST8
/* Recursive pointer members must not cause recursive type traversal. */
struct S { struct S *next; int x; };
void test (struct S *a, struct S *b)
{
  *a = *b;
}
#endif

#ifdef TEST9
/* Initialisation may supply a value for a const member. */
struct S { const int x; };
int test (int x)
{
  struct S s = {x};
  return s.x;
}
#endif

#ifdef TEST10
/* An ordinary member remains assignable beside a const member. */
struct S { const int x; int y; };
void test (struct S *a)
{
  a->y = 3;
}
#endif
