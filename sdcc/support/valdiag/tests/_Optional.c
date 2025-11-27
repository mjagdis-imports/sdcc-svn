#include <stddef.h>

// Basic warnings
#ifdef TEST1
_Optional int *p;
int *q;

void f1(void)
{
	p = q;
}

void f2(void)
{
	q = p; /* WARNING */
}

void g(int *r, _Optional int *s);

void h(void)
{
	g (q, q);
	g (p, p); /* WARNING */
	g (p, q); /* WARNING */
	g (q, p);
}
#endif

// _Optional, unlike other standard qualifiers, has defined semantics when applied to functions.
#ifdef TEST2
typedef void f_t(void);
_Optional f_t *fp1;
f_t *fp2;

void f1(void)
{
	fp1 = fp2;
}

void f2(void)
{
	fp2 = fp1; /* WARNING */
}
#endif

