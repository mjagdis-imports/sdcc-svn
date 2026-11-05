/* bug #4209: SDCC implements a freestanding environment. */
#include <testfwk.h>

void test_freestanding(void)
{
#ifdef __SDCC
  ASSERT(__STDC_HOSTED__ == 0);
#endif
}
