// A lot of undefined behavior from C23 has been removed in C2y
// Martin Uecker's "Ghosts and Demons" gives a good overview.
// The numbering of the test cases below corresponds to the numbering therein.
// In many cases, the resolution was to make it a constraint violation.

#ifdef TEST1b
extern int a;
_Static_assert(1 || a); /* ERROR(SDCC) */ /* WARNING(GCC) */
#endif

#ifdef TEST23
char foo_impl(int *p)
{
return p; /* WARNING */
}

char foo_expl(int *p)
{
return (char)p; /* WARNING */
}

_Bool foo_bool(int *p)
{
return (_Bool)p;
}
#endif

#ifdef TEST38a
struct A {
int a;
int x : (sizeof((struct A*)0)->a * 8); /* ERROR */
};
#endif

#ifdef TEST40
struct f { struct f *x; } *p;
void g(void)
{
(struct f)p; /* ERROR */
}
#endif

#ifdef TEST57a
void i(void)
{
static void f(void); /* ERROR */
}
#endif

#ifdef TEST57b
void i(void)
{
register void f(void); /* ERROR */
}
#endif


#ifdef TEST87
struct foo;
static struct foo x; /* ERROR */
#endif

