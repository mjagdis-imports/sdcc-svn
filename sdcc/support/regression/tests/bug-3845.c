/* bug-3845.c
   The mcs51 code generator failed to materialize a comparison result kept in
   the carry flag when storing it in a bool struct member.
 */

#include <testfwk.h>

#include <stdbool.h>

static int
bar (int value)
{
  return value;
}

void
testBug (void)
{
  struct
  {
    bool a;
  } foo;

  foo.a = bar (1) > 0;
  ASSERT (foo.a);

  foo.a = bar (0) > 0;
  ASSERT (!foo.a);
}
