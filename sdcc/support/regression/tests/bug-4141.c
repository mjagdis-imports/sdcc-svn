/* Inferring a function-pointer type must not alter the original parameters. */

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23_TESTS
#endif

static int calls;

static int
increment(int value)
#ifdef SDCC_PDK
__reentrant
#endif
{
  ++calls;
  return value + 1;
}

static int
decrement(int value)
#ifdef SDCC_PDK
__reentrant
#endif
{
  ++calls;
  return value - 1;
}

/* Reentrancy permits indirect calls with two int arguments on MCS51. */
static int
combine(int first, int second)
#ifdef __SDCC
__reentrant
#endif
{
  ++calls;
  return first * 10 + second;
}

void
testInferredFunctionParameters(void)
{
#ifndef SKIP_C23_TESTS
  /* Function conversion and an explicit address must both permit calls. */
  calls = 0;
  auto first = increment;
  auto second = &increment;
  ASSERT(first(4) == 5);
  ASSERT(second(6) == 7);

  /* Further inferred pointers must leave both earlier declarations usable. */
  auto third = first;
  auto other = decrement;
  ASSERT(third(8) == 9);
  ASSERT(other(10) == 9);
  ASSERT(first(12) == 13);
  ASSERT(second(14) == 15);

  /* Inference must also preserve the original functions' direct-call ABI. */
  ASSERT(increment(16) == 17);
  ASSERT(decrement(18) == 17);

  /* Check parameter order and values for a reentrant function type. */
  auto pair = combine;
  auto pair_address = &combine;
  ASSERT(pair(2, 3) == 23);
  ASSERT(pair_address(4, 5) == 45);
  ASSERT(combine(6, 7) == 67);
  ASSERT(calls == 11);
#endif
}
