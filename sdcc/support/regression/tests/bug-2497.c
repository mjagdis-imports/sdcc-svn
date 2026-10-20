/*
   bug-2497.c
 */

#include <testfwk.h>

#if !defined( __SDCC_pdk14) && !defined( __SDCC_pdk15) // pdk needs function pointer to be reentrant even for a single argument
typedef int func_t (int a);
typedef int (*func_p_t)(int a);

int fa (int a)
{
  return a + 1;
}

int fb (int b)
{
  return b + 10;
}

func_t *pa[2] = {fa, fb};
func_p_t pb[2] = {fb, fa};
#endif

/* The reopened report used a static function declaration through a typedef.
   Reentrancy also permits this two-argument indirect call on MCS51 and PDK. */
typedef long fnc_t(int a, long b) __reentrant;
static fnc_t fnc1;

static long fnc1(int a, long b) __reentrant
{
  return a + b;
}

fnc_t *fncp = fnc1;

void
testBug (void)
{
  ASSERT (fncp(1, 2) == 3);
#if !defined( __SDCC_pdk14) && !defined( __SDCC_pdk15)
  ASSERT (pa[0](5) == 6);
  ASSERT (pa[1](5) == 15);
  ASSERT (pb[0](15) == 25);
  ASSERT (pb[1](15) == 16);
#endif
}
