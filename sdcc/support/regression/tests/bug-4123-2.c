/* New tests for some existing z80 peephole optimizer rules that were no longer triggered by their original testcases.
   Tetscases found using Qwen3.8-27B-Q5_K_M */

#include <testfwk.h>

unsigned char gb;

void f21(void) // rule 21
{
  volatile unsigned char b;
  unsigned char a;
  b = 5;
  a = b;
  a >>= 1;
  b = a;
  gb = b;
}

void testf21(void)
{
  f21();
  ASSERT(gb == 2);
}

void g4(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

void f42(unsigned char *p) { // rule 42
    g4(p[0], p[1], p[2], p[3]);
}

void testf42(void)
{
  unsigned char c[4] = {1, 2, 3, 4};
  f42(c);
}

void g4(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
    ASSERT(a == 1);
    ASSERT(b == 2);
    ASSERT(c == 3);
    ASSERT(d == 4);
}

#include <stdint.h>
char x=1; void f77c(void){ char *p=&x; p=p+1; intptr_t i=(intptr_t)p; char *q=(char*)i; q=q-1; *q=11; }

void testf77c(void)
{
    f77c();
    ASSERT(x == 11);
}

