/* Short-circuiting permits calls in unevaluated constant subexpressions. */
#include <testfwk.h>

static unsigned char calls;

static int
called (void)
{
  calls++;
  return 5;
}

/* Each unexpected local-initialiser call identifies its own failure. */
static int
localAndCalled (void)
{
  ASSERT (0);
  return 5;
}

static int
localOrCalled (void)
{
  ASSERT (0);
  return 5;
}

static int zero = 0 && called ();
static int one = 1 || called ();
static int nested = (0 && called ()) || (1 || called ());
static int conditional = 1 ? 7 : called ();

void
testShortCircuitInitialisers (void)
{
  static int local_zero = 0 && localAndCalled ();
  static int local_one = 1 || localOrCalled ();

  /* Both the values and the absence of calls must be preserved. */
  ASSERT (zero == 0);
  ASSERT (one == 1);
  ASSERT (nested == 1);
  ASSERT (conditional == 7);
  ASSERT (local_zero == 0);
  ASSERT (local_one == 1);
  ASSERT (calls == 0);

  /* An evaluated left operand must retain its side effect. */
  ASSERT ((called () && 0) == 0);
  ASSERT ((called () || 1) == 1);
  ASSERT (calls == 2);
}
