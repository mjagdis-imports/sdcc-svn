// A lot of undefined behavior from C23 has been removed in C2y
// Martin Uecker's "Ghosts and Demons" gives a good overview.
// The numbering of the test cases below corresponds to the numbering therein.
// In many cases, the resolutionw as to make it a constraint violation.

#ifdef TEST1b
extern int a;
_Static_assert(1 || a); /* ERROR(SDCC) */ /* WARNING(GCC) */
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

#ifdef TEST87
struct foo;
static struct foo x; /* ERROR */
#endif

