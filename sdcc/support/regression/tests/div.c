/*  Test return of div functions

 */
#include <testfwk.h>

#include <stdlib.h>

void testDiv(void)
{
	ASSERT (div(4223, 23).quot == 4223 / 23);
	ASSERT (div(4223, 23).rem == 4223 % 23);
#if !defined(SDCC_PDK) && !(defined(__SDCC_mcs51) && defined(__SDCC_MODEL_SMALL))// Lack of memory
	ASSERT (ldiv(4223, 23).quot == 4223l / 23);
	ASSERT (ldiv(4223, 23).rem == 4223l % 23);
#if !defined(__SDCC_mos6502_stack_auto) && !defined(__SDCC_mos65c02_stack_auto) \
    && !defined(__SDCC_hc08) && !defined(__SDCC_s08) && !defined(__SDCC_s08_stack_auto) // no support for struct return with size > 8
	ASSERT (lldiv(4223, 23).quot == 4223ll / 23);
	ASSERT (lldiv(4223, 23).rem == 4223ll % 23);
#endif
#endif
}

