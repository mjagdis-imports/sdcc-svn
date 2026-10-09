/* Constant folding must agree with Z80 runtime division at signed minimum. */
#include <testfwk.h>

#ifdef __SDCC
#pragma disable_warning 165
#endif

void testConstantDivisionOverflow(void)
{
#ifdef __SDCC
  /* Folded quotient and remainder must not overflow the compiler host. */
  ASSERT((-2147483647L - 1L) / -1L == (-2147483647L - 1L));
  ASSERT((-2147483647L - 1L) % -1L == 0L);
  ASSERT((-9223372036854775807LL - 1LL) / -1LL == (-9223372036854775807LL - 1LL));
  ASSERT((-9223372036854775807LL - 1LL) % -1LL == 0LL);

  /* Volatile operands exercise runtime division rather than folding. */
  volatile long lmin = -2147483647L - 1L, lminusone = -1L;
  ASSERT(lmin / lminusone == (-2147483647L - 1L));
  ASSERT(lmin % lminusone == 0L);
#if !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15) // Lack of memory
  volatile long long llmin = -9223372036854775807LL - 1LL, llminusone = -1LL;
  ASSERT(llmin / llminusone == (-9223372036854775807LL - 1LL));
  ASSERT(llmin % llminusone == 0LL);
#endif
#endif
}
