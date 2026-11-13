/* Exact constexpr boolean initialisers and explicit boolean conversions. */
#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#ifndef SKIP_C23
static constexpr _Bool zero = 0, one = 1;
static constexpr _Bool positive = (_Bool)2, negative = (_Bool)-1;
static constexpr _Bool wide = (_Bool)0x100000000ULL;
static constexpr _Bool flags[] = {0, 1, (_Bool)2};
struct S { _Bool flag; int number; };
static constexpr struct S object = {(_Bool)2, 2};
struct Bits { _Bool flag : 1; };
static constexpr struct Bits bits = {(_Bool)-1};
union U { _Bool flag; int number; };
static constexpr union U number = {.number = 2};
#endif

void
test_constexpr_bool (void)
{
#ifndef SKIP_C23
  constexpr _Bool local = (_Bool)2;
  constexpr _Bool locals[] = {0, 1, (_Bool)-1};
  constexpr struct S aggregate = {(_Bool)-1, 3};
  constexpr struct Bits localbits = {(_Bool)2};

  ASSERT (zero == 0);
  ASSERT (one == 1);
  ASSERT (positive == 1);
  ASSERT (negative == 1);
  ASSERT (wide == 1);
  ASSERT (flags[0] == 0);
  ASSERT (flags[1] == 1);
  ASSERT (flags[2] == 1);
  ASSERT (object.flag == 1);
  ASSERT (object.number == 2);
  ASSERT (bits.flag == 1);
  ASSERT (number.number == 2);
  ASSERT (local == 1);
  ASSERT (locals[0] == 0);
  ASSERT (locals[1] == 1);
  ASSERT (locals[2] == 1);
  ASSERT (aggregate.flag == 1);
  ASSERT (aggregate.number == 3);
  ASSERT (localbits.flag == 1);
#endif
}
