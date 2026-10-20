/* Subscripts in designated initialisers must be integer constant expressions. */
#ifdef TEST1
int a[3] = {[1.5] = 1}; /* ERROR */
#endif

#ifdef TEST2
int a[3] = {[1.0] = 1}; /* ERROR */
#endif

#ifdef TEST3
int a[3] = {[0.5 + 0.5] = 1}; /* ERROR */
#endif

#ifdef TEST4
int a[3] = {[(float)1] = 1}; /* ERROR */
#endif

#ifdef TEST5
int a[2][2] = {[1][0.5] = 1}; /* ERROR */
#endif

#ifdef TEST6
struct S { int a[3]; };
struct S s = {.a[1.5] = 1}; /* ERROR */
#endif

/* Valid test cases: integer expressions and an explicit cast to int. */
#ifdef TEST7
int a[3] = {[1] = 1};
int b[3] = {[(int)1.5] = 1};
int c[3] = {[sizeof(char)] = 1};
#endif

/* Valid test cases: nested arrays and an array member. */
#ifdef TEST8
int a[2][2] = {[1][0] = 1};
struct S { int a[3]; };
struct S s = {.a[1] = 1};
#endif

#ifdef TEST9
int a[3] = {[-1] = 1}; /* ERROR */
#endif

/* A nonconstant subscript must still be diagnosed. */
#ifdef TEST10
void f(int subscript)
{
  int a[3] = {[subscript] = 1}; /* ERROR */
}
#endif

/* Ordinary array subscripting uses the same diagnostic. */
#ifdef TEST11
void f(void)
{
  int a[3];
  a[1.5] = 1; /* ERROR */
}
#endif
