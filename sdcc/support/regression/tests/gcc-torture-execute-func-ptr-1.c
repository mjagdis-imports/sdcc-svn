/*
   func-ptr-1.c from the execute part of the gcc torture tests.
 */

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c99
#endif

static double f (float a) __reentrant;
static double (*fp) (float a) __reentrant;

void
testTortureExecute (void)
{
  fp = f;
  if (fp ((float) 1) != 1.0)
    ASSERT (0);
  return;
}

static double
f (float a) __reentrant
{
  return a;
}
