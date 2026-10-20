/* Return-context conversion of constexpr integer initialisers to float. */
#include <testfwk.h>
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23_TESTS
#endif

#ifndef SKIP_C23_TESTS
/* Keep the return context of the reproducer separate from the assertions.
   A direct assignment from the same constexpr object can pass before the fix. */

static float
negativeInt(void)
{
  constexpr float value = -1;
  return value;
}

static float
negativeLong(void)
{
  constexpr float value = -2L;
  return value;
}

static float
negativeLongLong(void)
{
  constexpr float value = -3LL;
  return value;
}

static float
negativeChar(void)
{
  constexpr float value = (signed char)-4;
  return value;
}

static float
negativeBitInt(void)
{
  constexpr float value = (_BitInt(17))-5;
  return value;
}

static float
negativeObject(void)
{
  constexpr int source = -9;
  constexpr float value = source;
  return value;
}

static float
positiveUnsigned(void)
{
  constexpr float value = 65535U;
  return value;
}

static float
positiveInteger(void)
{
  constexpr float value = 42;
  return value;
}

static float
zero(void)
{
  constexpr float value = 0;
  return value;
}

static float
negativeFloat(void)
{
  constexpr float value = -1.0f;
  return value;
}

static float
explicitCast(void)
{
  constexpr float value = (float)-1;
  return value;
}

static int
negativeInteger(void)
{
  constexpr int value = -7;
  return value;
}

#endif /* !SKIP_C23_TESTS */

void
testConstexprNegativeFloat(void)
{
#ifndef SKIP_C23_TESTS
  /* These six return-context cases fail before the conversion fix. */
  volatile float result = negativeInt();
  ASSERT(result == -1.0f);
  result = negativeLong();
  ASSERT(result == -2.0f);
  result = negativeLongLong();
  ASSERT(result == -3.0f);
  result = negativeChar();
  ASSERT(result == -4.0f);
  result = negativeBitInt();
  ASSERT(result == -5.0f);
  result = negativeObject();
  ASSERT(result == -9.0f);

  /* These six valid cases pass both before and after the fix. */
  result = positiveUnsigned();
  ASSERT(result == 65535.0f);
  result = positiveInteger();
  ASSERT(result == 42.0f);
  result = zero();
  ASSERT(result == 0.0f);
  result = negativeFloat();
  ASSERT(result == -1.0f);
  result = explicitCast();
  ASSERT(result == -1.0f);
  ASSERT(negativeInteger() == -7);
#endif /* !SKIP_C23_TESTS */
}
