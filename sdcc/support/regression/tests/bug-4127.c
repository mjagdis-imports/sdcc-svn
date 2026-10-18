/* Valid boundary values must retain all 64 bits when read. */
#include <testfwk.h>

void testLiteralBoundaries (void)
{
  volatile unsigned long long value;

  /* Unsigned addition wraps at 2^64, independently of a limits macro. */
  value = 18446744073709551615ULL;
  ASSERT (value + 1ULL == 0ULL);
  value = 0xffffffffffffffffULL;
  ASSERT (value + 1ULL == 0ULL);
  value = 01777777777777777777777ULL;
  ASSERT (value + 1ULL == 0ULL);

  /* Convert the signed maximum before adding, to avoid signed overflow. */
  value = 9223372036854775807LL;
  ASSERT (value + 1ULL == (1ULL << 63));
  value = 18446744073709551614ULL;
  ASSERT (value + 2ULL == 0ULL);

  /* Leading zeros must not affect a small value. */
  value = 00000000000000000000000000000000000000000000000001ULL;
  ASSERT (value == 1ULL);
}
