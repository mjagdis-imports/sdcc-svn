/* typeof of a scalar constant must permit a modifiable object. */

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
/* String inference produces char *, so warning 360 is expected below. */
#pragma disable_warning 360
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23_TESTS
#endif

void
testLiteralType(void)
{
#ifndef SKIP_C23_TESTS
  typeof(nullptr) null_value = nullptr;
  typeof(0) integer_value = 0;
  typeof(true) boolean_value = true;
  typeof(1 + 2) sum_value = 3;

  null_value = nullptr;
  integer_value = 7;
  boolean_value = false;
  sum_value = 9;

  ASSERT(integer_value == 7);
  ASSERT(boolean_value == false);
  ASSERT(sum_value == 9);
  ASSERT(_Generic(&null_value, typeof(nullptr) *: 1, default: 0));
  ASSERT(_Generic(&integer_value, int *: 1, default: 0));
#endif
}
