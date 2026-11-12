/* Preserve valid constexpr integer initialisation and explicit conversions. */
#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#ifndef SKIP_C23
static constexpr int converted = (int)1.5;
static constexpr int integer_expression = 2 + 3;
struct S { int x; int y; };
static constexpr struct S aggregate = {1, (int)2.5};
#endif

void
testConstexprIntegerInitialisers (void)
{
#ifndef SKIP_C23
  const volatile int *p = &converted;
  const volatile struct S *s = &aggregate;
  constexpr int local = (int)3.5;
  ASSERT (*p == 1);
  p = &integer_expression;
  ASSERT (*p == 5);
  ASSERT (s->x == 1);
  ASSERT (s->y == 2);
  ASSERT (local == 3);
#endif
}
