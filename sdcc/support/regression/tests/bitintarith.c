/** bit-precise integers

    width: 2, 4, 7, 8, 9, 15, 16, 17, 24, 33, 40, 48, 63, 64, 65
*/

#include <testfwk.h>

#if __SDCC_BITINT_MAXWIDTH >= {width} // TODO: When we can regression-test in --std-c23 mode, use the standard macro from limits.h instead!

#endif

void testBitIntArith(void)
{
#if __SDCC_BITINT_MAXWIDTH >= {width} // TODO: When we can regression-test in --std-c23 mode, use the standard macro from limits.h instead!

#endif
}

