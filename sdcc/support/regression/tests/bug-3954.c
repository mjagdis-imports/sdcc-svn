/* bug-3954.c
   Array-to-pointer conversion must preserve the qualified
   element type.
 */

#include <testfwk.h>

void
testBug (void)
{
  const char (*p)[64] = 0;

  ASSERT (_Generic (*p, const char *: 1, default: 0));
}
