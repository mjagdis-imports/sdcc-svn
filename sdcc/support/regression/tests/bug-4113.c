/* Literal comparison must account for bits above int and long widths. */
#include <testfwk.h>

_Static_assert (0x20000000000000ULL != 0x20000000000001ULL, "wide literals");
_Static_assert (0x20000000000000ULL < 0x20000000000001ULL, "less");
_Static_assert (0x20000000000001ULL > 0x20000000000000ULL, "greater");
_Static_assert (!(0x20000000000001ULL <= 0x20000000000000ULL), "less or equal");
_Static_assert (!(0x20000000000000ULL >= 0x20000000000001ULL),
                "greater or equal");
#if defined(__SDCC) && __SDCC_BITINT_MAXWIDTH >= 40
_Static_assert ((unsigned _BitInt (17))65536 != 0, "17-bit literal");
_Static_assert ((unsigned _BitInt (17))131071 == -1, "17-bit conversion");
_Static_assert ((unsigned _BitInt (40))0x100000000ULL != 0, "40-bit literal");
#endif

static inline _Bool narrowedDifference (unsigned int *p,
                                       signed long long a, signed long long b)
{
  signed long long result = a - b;
  *p = result;
  return *p != result;
}

#ifndef __SDCC_pdk14 // Lack of memory
static _Bool mixedSignQuotientsEqual (long dividend)
{
  return dividend / 2L == dividend / 2UL;
}
#endif

void testLongLongEquality (void)
{
  unsigned int r;
  unsigned long long a = 0x69aaaaaaaaaa55aaULL;
  unsigned long long b = 0x69555555555555aaULL;
  volatile unsigned long long va = 0x69aaaaaaaaaa55aaULL;
  volatile unsigned long long vb = 0x69555555555555aaULL;
  unsigned long long high = 0x20000000000000ULL;
  unsigned long long next = 0x20000000000001ULL;

  /* Equal literal values do not make signed and unsigned division equal. */
#ifndef __SDCC_pdk14 // Lack of memory
  ASSERT (mixedSignQuotientsEqual (4L));
  ASSERT (!mixedSignQuotientsEqual (-4L));
  ASSERT (!mixedSignQuotientsEqual (-1L));
#endif

  ASSERT (0x10000LL != 0);
  ASSERT (!(0x10000LL == 0));
  ASSERT (0x100000000LL != 0);
  ASSERT (!(0x100000000LL == 0));
  ASSERT (high != next);
  ASSERT (!(high == next));
  ASSERT (high == 0x20000000000000ULL);
  ASSERT (!(high != 0x20000000000000ULL));
  ASSERT (high < next);
  ASSERT (next > high);
  ASSERT (!(next <= high));
  ASSERT (!(high >= next));
  ASSERT (high <= high);
  ASSERT (high >= high);
  ASSERT (-0x20000000000001LL < -0x20000000000000LL);
  ASSERT (0x8000000000000000ULL > 0x7fffffffffffffffULL);
  ASSERT (!(0x7fffffffffffffffULL > 0x8000000000000000ULL));
  ASSERT ((unsigned int)-1 > -1LL);
  ASSERT ((unsigned int)-1 == -1);
  ASSERT ((unsigned char)200 > (signed char)-1);

  /* Constant propagation must agree with runtime bitwise operations. */
  ASSERT ((a & b) == 0x69000000000055aaULL);
  ASSERT ((a | b) == 0x69ffffffffff55aaULL);
  ASSERT ((a ^ b) == 0x00ffffffffff0000ULL);
  ASSERT ((a & b) == (va & vb));
  ASSERT ((a | b) == (va | vb));
  ASSERT ((a ^ b) == (va ^ vb));

  /* A long long operand does not make a floating comparison integral. */
  ASSERT (1LL == 1.0f);
  ASSERT (!(1LL != 1.0f));
  ASSERT (1LL != 1.5f);
  ASSERT (!(1LL == 1.5f));

  ASSERT (narrowedDifference (&r, 100, 200));
  ASSERT (r == (unsigned int)-100);
}
