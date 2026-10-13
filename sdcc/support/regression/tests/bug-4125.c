/* Valid integer counts must keep their existing shift results. */
#include <testfwk.h>

void testIntegerShiftCounts (void)
{
  volatile unsigned int value = 12;
  volatile unsigned char count = 1;

  ASSERT ((value << count) == 24);
  ASSERT ((value >> count) == 6);
  value <<= count;
  ASSERT (value == 24);
  value >>= count;
  ASSERT (value == 12);
}
