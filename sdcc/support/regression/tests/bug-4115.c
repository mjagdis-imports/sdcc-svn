/* A faulty z80 peephole optimizer rule. */

#include <testfwk.h>

void h(unsigned int a, unsigned int b);

void f(unsigned char *p)
{
    *p = 1;
    h(0, 0x00ff);
}

void testBug(void)
{
    unsigned char c = 0;
    f(&c);
    ASSERT(c == 1);
}

void h(unsigned int a, unsigned int b)
{
    ASSERT(a == 0);
    ASSERT(b == 0x00ff);
}

