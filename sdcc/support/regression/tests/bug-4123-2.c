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

unsigned char gv;
void barrier(void);
void f76(void) { // rule 76
    unsigned char arr[3], pad[2];
    arr[2] = gv;
    pad[0] = 1; pad[1] = 2;
    barrier();
    arr[2] |= 4;
    gv = arr[2];
}

void testf76(void)
{
    gv = 2;
    f76();
    ASSERT(gv == 6);
}

void barrier(void)
{
}

#include <stdint.h>
#pragma disable_warning 127
char obj[8] = {1, 2, 3, 4, 5, 6, 7, 8};
void f77a(void) { // rule 77a
    char *p = obj;               /* p = &obj[0]  (inside obj) */
    p = p + 3;                   /* p = &obj[3]  (inside obj) */
    intptr_t i = (intptr_t)p;    /* break constant folding */
    char *q = (char*)i;
    q++;                         /* q = &obj[4]  (inside obj) */
    *q = 11;
}

void testf77a(void)
{
    f77a();
    ASSERT(obj[4] == 11);
}

#include <stdint.h>
char x=1; void f77c(void){ char *p=&x; p=p+1; intptr_t i=(intptr_t)p; char *q=(char*)i; q=q-1; *q=11; }

void testf77c(void)
{
    f77c();
    ASSERT(x == 11);
}

