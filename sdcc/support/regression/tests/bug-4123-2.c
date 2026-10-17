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

void test21(void)
{
  f21();
  ASSERT(gb == 2);
}

void g4(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

void f42(unsigned char *p) { // rule 42
    g4(p[0], p[1], p[2], p[3]);
}

void test42(void)
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


void barrier(void);
void f76(void) { // rule 76
    unsigned char arr[3], pad[2];
    arr[2] = gb;
    pad[0] = 1; pad[1] = 2;
    barrier();
    arr[2] |= 4;
    gb = arr[2];
}

void test76(void)
{
    gb = 2;
    f76();
    ASSERT(gb == 6);
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

void test77a(void)
{
    f77a();
    ASSERT(obj[4] == 11);
}

#include <stdint.h>
char x=1; void f77c(void){ char *p=&x; p=p+1; intptr_t i=(intptr_t)p; char *q=(char*)i; q=q-1; *q=11; }

void test77c(void)
{
    f77c();
    ASSERT(x == 11);
}

void f102d(unsigned char *p, unsigned char n) { // rule 102
    *p -= n;
    if (*p) { *p = 1; }
}

void test102d(void)
{
    unsigned char p[2] = {1, 3};
    f102d(p + 1, 1);
    ASSERT(p[1] == 1);
    f102d(p + 1, 1);
    ASSERT(p[1] == 0);
}

int g105(int a, char b, char c, char d, char e);

int f105(int x) // rule 105
{
    return g105(x*3, (char)(x>>8), (char)(x&0xff),
             (char)(x>>8), (char)(x&0xff)) + x;
}

void test105(void)
{
    ASSERT(f105(0x102) == 0x40e);
}

int g105(int a, char b, char c, char d, char e)
{
    return a + b + c + d + e;
}

void f131b(char *bp, unsigned n) { // rule 131b
  register char *ep = bp + n, *sp;
  while (bp < ep) {
    sp = bp + 2;
    *ep++ = *sp;
    sp = bp + 1;
    *sp = *sp;
    bp += 2;
  }
}

void test131b(void)
{
  char a[3] = {1, 2, 3};
  f131b(a, 1);
  ASSERT(a[1] == 3);
}

int f140a(int x) { return x + 1; }
int G;
void m140a(void) { // rule 140a
  int t = f140a(1) + f140a(2);
  G = t + t;
}

void test140a(void)
{
  m140a();
  ASSERT(G == 10);
}

