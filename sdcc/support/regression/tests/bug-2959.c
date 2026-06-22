/*
   bug-2959.c

   The pdk backends tried to do a 16-bit read from an __sfr instead of an 8-bit read followed by a cast.
*/

#include <testfwk.h>

// Test for all backends that can have an __sfr at 0x10.
#if (defined(__SDCC_pdk13) || defined(__SDCC_pdk14) || defined(__SDCC_pdk15) || defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_ez80))
__sfr __at(0x10) _pa;
#elif defined(__SDCC_mcs51)
__sfr __at(0x99) _pa;
#else
unsigned char _pa;
#endif

void f(void)
{
  if( 0x10 == (_pa & 0x10) ) // Code generation crash here.
  { 
#if defined(__SDCC_f8) || defined(__SDCC_f8l) // Neither f8 nor f8l have nop
    __asm__("xch f, (0,sp)\n");
    __asm__("xch f, (0,sp)\n");
#else
    __asm__("nop\n");
#endif
  }
}

void testBug(void)
{
}

