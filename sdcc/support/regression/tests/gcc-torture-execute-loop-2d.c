/*
   loop-2d.c from the execute part of the gcc torture tests.
 */

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c99
#pragma disable_warning 322 // not on all platforms does a pointer fit in an int
                            // shouldn´t an explicit cast like used here suppress that warning?
#endif

int a[2];

void f (int b)
{
  unsigned int i;
  int *p;
  for (p = &a[b], i = b; --i < ~0; )
    *--p = i * 3 + (int)a;
}

void
testTortureExecute (void)
{
  a[0] = a[1] = 0;
  f (2);
  if (a[0] != (int)a || a[1] != (int)a + 3)
    ASSERT (0);
  return;
}
