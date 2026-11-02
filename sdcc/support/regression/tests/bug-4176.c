/* A fixed-underlying forward declaration establishes the enumeration's type. */
#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#ifndef SKIP_C23
enum E : unsigned char;
static enum E before_definition;
enum E : unsigned char { limit = 255 };
static enum E after_definition = limit;
#endif

void
testFixedEnumForwardDeclaration (void)
{
#ifndef SKIP_C23
  ASSERT (sizeof before_definition == sizeof (unsigned char));
  ASSERT (sizeof after_definition == sizeof (unsigned char));
  ASSERT (_Generic ((enum E)0, unsigned char: 1, default: 0));
  before_definition = limit;
  ASSERT (before_definition == 255);
  ASSERT (after_definition == 255);
#endif
}
