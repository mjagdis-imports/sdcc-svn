/* New tests for some existing z80 peephole optimizer rules that were no longer triggered by their original testcases.
   Tetscases found using Qwen3.8-27B-Q5_K_M */

#include <testfwk.h>

unsigned char gb;

void f(void) // rule 21
{
  volatile unsigned char b;
  unsigned char a;
  b = 5;
  a = b;
  a >>= 1;
  b = a;
  gb = b;
}

void testf(void)
{
  f();
  ASSERT(gb == 2);
}

