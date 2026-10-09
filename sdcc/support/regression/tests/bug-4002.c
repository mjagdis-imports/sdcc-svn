/* Pointer subtraction removes _Optional from the referenced type while
   preserving the other qualifiers. */

#include <testfwk.h>

void
testBug (void)
{
#ifdef __SDCC // todo: enable for host compilers once there is a way to check for _Optional support.
  _Optional const volatile int *p = 0;
  int *const volatile _Optional restrict *pp = 0;

  ASSERT (_Generic (p - 0, const volatile int *: 1, default: 0));
  ASSERT (_Generic (pp - 0,
                    int *const volatile restrict *: 1,
                    default: 0));
#endif
}
