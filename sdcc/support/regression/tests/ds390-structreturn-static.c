/* Test ds390 struct return from static storage. */
#include <testfwk.h>

#if defined(__SDCC_ds390)
struct pair { unsigned char a, b; };
static struct pair source = { 41, 42 };

static struct pair staticPair (void)
{
  return source;
}

static struct pair (*volatile indirectPair)(void) = staticPair;
#endif

void testStructReturnStatic (void)
{
#if defined(__SDCC_ds390)
  struct pair result;
  result = staticPair ();
  ASSERT (result.a == 41);
  ASSERT (result.b == 42);

  source.a = 51;
  source.b = 52;
  result.a = 0;
  result.b = 0;
  result = indirectPair ();
  ASSERT (result.a == 51);
  ASSERT (result.b == 52);
#endif
}
