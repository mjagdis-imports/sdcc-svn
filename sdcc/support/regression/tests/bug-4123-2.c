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

int f127b(unsigned char y, unsigned char zg) {
    unsigned char t;
    t = gb - y;
    gb = t;
    if (t == 0) {
        if (zg) return 2;
        return 0;
    }
    return 1;
}

void test127b(void)
{
    gb = 2;
    ASSERT(f127b(1, 1) == 1);
    ASSERT(gb == 1);
    ASSERT(f127b(1, 1) == 2);
    ASSERT(gb == 0);
}

#if !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15) && !defined(__SDCC_mcs51) // Lack of memory.
#pragma disable_warning 85
int f130c(void) {
    long a[70];
    volatile long x, y;
    x = 0x12345678UL;
    y = 1L;
    return (int)(x >> 16);
}
#endif

void test130x(void)
{
#if !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15) && !defined(__SDCC_mcs51) // Lack of memory.
    ASSERT(f130c() == 0x1234);
#endif
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

#if defined(__SDCC_stm8) || defined(__SDCC_z80) || defined(__SDCC_z80n) || defined(__SDCC_z180) || defined(__SDCC_r2k) || defined(__SDCC_r3ka) || defined(__SDCC_sm83)
#define SDCCCALL0 __sdcccall(0)
#else
#define SDCCCALL0
#endif

int G1, G2;
void h144(char a, int b) SDCCCALL0 { G1 = a + b; }
void g144(int x) SDCCCALL0 { G2 = x; }
void f144(void) SDCCCALL0 { // rule 144
    h144(1, 2);
    g144(42);
}

void test144(void)
{
    f144();
    ASSERT(G1 == 3);
    ASSERT(G2 == 42);
}

void h145(char a, int b) SDCCCALL0 { G1 = a + b; }
void g145(char c1, char c2) SDCCCALL0 { G2 = c1 + c2; }
void f145(int d1, char p3, char p4) SDCCCALL0 { // rule 145
    p3 += d1 & p4;
    p4 += p3;
    h145(1, 2);
    g145(p3, p4);
}

void test145(void)
{
    f145(3, 1, 6);
    ASSERT(G1 == 3);
    ASSERT(G2 == 12);
}

void h146(int a) SDCCCALL0 { G1 = a; }
void g146(char c) SDCCCALL0 { G2 = c; }
void f146(void) SDCCCALL0 {
    h146(100);
    g146(42);
}

void test146(void)
{
    f146();
    ASSERT(G1 == 100);
    ASSERT(G2 == 42);
}

unsigned long G32;

void f194_2(void) // rule 194-2
{
  register int w = G2;
  unsigned long v = G32;
  if (v >= 0x1234)
    G1 = 1;
  G1 = w;
}

void test194_2(void)
{
  G2 = 3;
  G32 = 0x1234;
  f194_2();
  ASSERT(G1 == 3);
}

