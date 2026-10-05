/*
   Constant conditional expressions must test the full scalar value.
*/
#include <testfwk.h>
static int wide(void) { return 4294967296ULL ? 1 : 0; }
static int fractional(void) { return 0.5f ? 1 : 0; }
void testConditionalTruth(void)
{
  ASSERT(wide() == 1);
  ASSERT(fractional() == 1);
  ASSERT((0ULL ? 1 : 0) == 0);
}
