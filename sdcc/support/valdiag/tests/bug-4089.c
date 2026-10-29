/* N3886 6.5.5p2: operands of casts to non-void types must be scalar. */

#ifdef TEST1
struct S { int x; } s;
int test(void) { return (int)s; /* ERROR */ }
#endif

#ifdef TEST2
union U { int x; } u;
int test(void) { return (int)u; /* ERROR */ }
#endif

#ifdef TEST3
struct S { int x; } s;
void test(void) { (void)s; }
#endif

#ifdef TEST4
union U { int x; } u;
void test(void) { (void)u; }
#endif

#ifdef TEST5
int a[2];
int *test(void) { return (int *)a; }
#endif

#ifdef TEST6
void f(void);
void (*test(void))(void) { return (void (*)(void))f; }
#endif

