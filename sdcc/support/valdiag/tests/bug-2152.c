/* Bug #2152: diagnose overflow in the reported delay calculation. */
#include <stdint.h>

#ifdef TEST1C99
#define F_CPU 4000000
#define DIV_idelay ((uint16_t)4*16*256)
#define delay_mstoi(time) ((((int32_t)(F_CPU))*(time)+((int32_t)1000*DIV_idelay/2))/ ((int32_t)1000*DIV_idelay)) /* WARNING (GCC) */

uint8_t tmp8;
uint16_t tmp16;

void f(void)
{
  tmp8 = delay_mstoi(3000); /* WARNING (SDCC) */ /* IGNORE (GCC) */
  tmp16 = delay_mstoi(3000); /* WARNING (SDCC) */ /* IGNORE (GCC) */
}
#endif
