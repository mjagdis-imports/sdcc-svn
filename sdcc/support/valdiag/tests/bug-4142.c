/* Array length expressions require integer type, regardless of their numerical value. */
#ifdef TEST1
int a[2.5]; /* ERROR */
#endif

#ifdef TEST2
int a[2.0]; /* ERROR */
#endif

#ifdef TEST3
int a[1.0 + 1.0]; /* ERROR */
#endif

#ifdef TEST4
typedef int A[2.5]; /* ERROR */
#endif

#ifdef TEST5
struct S { int a[2.5]; }; /* ERROR */
#endif

#ifdef TEST6
void f(int a[2.5]); /* ERROR */
#endif

#ifdef TEST7
unsigned f(void)
{
  return sizeof(int[2.5]); /* ERROR */
}
#endif

#ifdef TEST8
void f(int (*p)[2.5]); /* ERROR */
#endif

/* Valid test cases: integer constant expressions. */
#ifdef TEST9
int a[2];
typedef int A[1 + 1];
unsigned f(void)
{
  return sizeof(int[2]) + sizeof(A) + sizeof(a);
}
#endif

/* An explicit cast to int makes the length expression valid. */
#ifdef TEST10
int a[(int)2.5];
unsigned f(void)
{
  return sizeof(int[(int)2.5]) + sizeof(a);
}
#endif

/* Valid test cases: sizeof and a comparison have integer type. */
#ifdef TEST11
int a[sizeof(float)];
int b[1 < 2];
#endif

/* A negative integer length retains its existing diagnostic. */
#ifdef TEST12
int a[-1]; /* ERROR */
#endif

/* A block-scope declaration requires the same type check. */
#ifdef TEST13
void f(void)
{
  int a[2.5]; /* ERROR */
  (void)a;
}
#endif

/* A negative value of floating-point type must fail the type check. */
#ifdef TEST14
int a[-0.5]; /* ERROR */
#endif

/* The same check applies to an abstract array declarator. */
#ifdef TEST15
unsigned f(void)
{
  return sizeof(int[-0.5]); /* ERROR */
}
#endif
