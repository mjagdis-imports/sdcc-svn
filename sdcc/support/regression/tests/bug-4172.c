/* Regression test for bug #4172 (untested rab peephole optimizer rules)
   Rule 102c (sbc hl / ld a,h / or a,l / jp NZ -> sbc hl / jp NZ),
   rule 129rab (inc hl / ld %1,#%2 / add hl,%1 -> ld %1,#%2+1 / add hl,%1)
   and rule 129rab' (same with dec hl) were not triggered by any
   regression test.
   Testcases found using Qwen3.8 */

#include <testfwk.h>

static unsigned sink102c;

static void show102c(unsigned t)
{
    sink102c = t;
}

unsigned f102c(unsigned a, unsigned b) // rule 102c
{
    unsigned d = a - b;
    unsigned t = 3 - d;
    if (t) show102c(t);
    show102c(t);
    return t;
}

unsigned char *f129rab(unsigned char *p) // rule 129rab
{
    p++;
    p += 5;
    *p = 7;
    return p;
}

unsigned f129rabp(unsigned x) // rule 129rab'
{
    x--;
    x += (unsigned)-5;
    return x;
}

void test102c(void)
{
    ASSERT(f102c(0, 0) == 3);
    ASSERT(f102c(1, 0) == 2);
    ASSERT(f102c(3, 0) == 0);
    ASSERT(f102c(1, 2) == 4);
    ASSERT(f102c(100, 20) == (unsigned)(3 - 80));
    ASSERT(sink102c == (unsigned)(3 - 80));
}

void test129rab(void)
{
    unsigned char buf[8];
    unsigned char *p = f129rab(buf);
    ASSERT(p - buf == 6);
    ASSERT(*p == 7);
}

void test129rabp(void)
{
    ASSERT(f129rabp(10) == 4);
    ASSERT(f129rabp(6) == 0);
    ASSERT(f129rabp(0) == (unsigned)(-6));
    ASSERT(f129rabp(100) == 94);
}
