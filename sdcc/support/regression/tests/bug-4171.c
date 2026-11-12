/* Regression test for bug #4171 (untested sm83 peephole optimizer rules)
   Rules 1, 12, 13, 14, 19, sp3, sp6, sp7 and sp8b were not triggered by
   any regression test.
   Testcases found using Qwen3.8 */

#include <testfwk.h>

unsigned char gvA1;
void gv(unsigned char a1, ...)
{
    gvA1 = a1;
}

unsigned int g2A1;
void g2(unsigned int a1, ...)
{
    g2A1 = a1;
}

unsigned int g19A1, g19A2;
unsigned char g19A3, g19A4;
void g19(unsigned int a1, unsigned int a2, unsigned char a3, unsigned char a4)
{
    g19A1 = a1;
    g19A2 = a2;
    g19A3 = a3;
    g19A4 = a4;
}

#if defined(__SDCC_sm83) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_ez80) || defined(__SDCC_tlcs90) || defined(__SDCC_huc6280) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) || defined(__SDCC_mcs51) // 0xff10 plain writable RAM only on these ports (0xff00 is XPC on Rabbit, flash on HC08/S08)

struct S1 { unsigned char x : 8; };

void f1(unsigned char c) // rule 1
{
    ((volatile struct S1 *)0xff10)->x = c;
}

void f12(void) // rule 12
{
    (*(volatile unsigned char *)0xff10)++;
}

void f13(void) // rule 13
{
    (*(volatile unsigned char *)0xff10)--;
}

void f14(void) // rule 14
{
    (*(volatile unsigned char *)0xff10) >>= 1;
}

#endif

void test1(void)
{
#if defined(__SDCC_sm83) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_ez80) || defined(__SDCC_tlcs90) || defined(__SDCC_huc6280) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) || defined(__SDCC_mcs51) // 0xff10 plain writable RAM only on these ports (0xff00 is XPC on Rabbit, flash on HC08/S08)
    f1(0x5a);
    ASSERT(((volatile struct S1 *)0xff10)->x == 0x5a);
#endif
}

void test12(void)
{
#if defined(__SDCC_sm83) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_ez80) || defined(__SDCC_tlcs90) || defined(__SDCC_huc6280) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) || defined(__SDCC_mcs51) // 0xff10 plain writable RAM only on these ports (0xff00 is XPC on Rabbit, flash on HC08/S08)
    *(volatile unsigned char *)0xff10 = 0x55;
    f12();
    ASSERT(*(volatile unsigned char *)0xff10 == 0x56);
#endif
}

void test13(void)
{
#if defined(__SDCC_sm83) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_ez80) || defined(__SDCC_tlcs90) || defined(__SDCC_huc6280) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) || defined(__SDCC_mcs51) // 0xff10 plain writable RAM only on these ports (0xff00 is XPC on Rabbit, flash on HC08/S08)
    *(volatile unsigned char *)0xff10 = 0x55;
    f13();
    ASSERT(*(volatile unsigned char *)0xff10 == 0x54);
#endif
}

void test14(void)
{
#if defined(__SDCC_sm83) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_ez80) || defined(__SDCC_tlcs90) || defined(__SDCC_huc6280) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) || defined(__SDCC_mcs51) // 0xff10 plain writable RAM only on these ports (0xff00 is XPC on Rabbit, flash on HC08/S08)
    *(volatile unsigned char *)0xff10 = 0x55;
    f14();
    ASSERT(*(volatile unsigned char *)0xff10 == 0x2a);
#endif
}

unsigned char f19(unsigned char w) // rule 19
{
    g19(0x1234, 0x5678, 0xC0, w);
    return 1;
}

void test19(void)
{
    ASSERT(f19(0x42) == 1);
    ASSERT(g19A1 == 0x1234);
    ASSERT(g19A2 == 0x5678);
    ASSERT(g19A3 == 0xC0);
    ASSERT(g19A4 == 0x42);
}

void fsp3(void) // sp3
{
    gv('a', 'b', 'c');
    for(;;) ;
}

void testsp3(void)
{
    /* fsp3 deliberately never returns (the trailing loop is what keeps sp
       unused after the stack cleanup, as sp3 requires), so gv is checked
       by calling it directly. */
    gv('a', 'b', 'c');
    ASSERT(gvA1 == 'a');
}

void fsp6(void) // sp6
{
    g2(0x1234);
    for(;;) ;
}

void testsp6(void)
{
    /* fsp6 deliberately never returns (same reason as fsp3), so g2 is
       checked by calling it directly. */
    g2(0x1234);
    ASSERT(g2A1 == 0x1234);
}

void fsp7(unsigned char a) // sp7
{
    volatile unsigned char l1, l2;
    l1 = a;
    l2 = l1;
    gv(l2);
}

void testsp7(void)
{
    fsp7(0x21);
    ASSERT(gvA1 == 0x21);
}

void fsp8b(unsigned char a) // sp8b
{
    volatile unsigned char l1, l2, l3, l4, l5;
    l1 = a;
    l2 = l1;
    l3 = l2;
    l4 = l3;
    l5 = l4;
    gv(l5);
}

void testsp8b(void)
{
    fsp8b(0x3c);
    ASSERT(gvA1 == 0x3c);
}
