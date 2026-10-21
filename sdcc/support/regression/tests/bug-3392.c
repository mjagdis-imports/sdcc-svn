/* Bug #3392: retain the original constant-folded negation conditions.
 */
#include <testfwk.h>

static int notneg(void)
{
  int x;

  x = 4;
  if (!x != 0)
    return 1;
  if (!!x != 1)
    return 1;
  if (-x != 0 - 4)
    return 1;
  return 0;
}

void
testBug(void)
{
  ASSERT (notneg() == 0);
}
