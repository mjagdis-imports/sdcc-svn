/* Array-to-pointer conversion removes _Optional from the referenced type of
   the result, while preserving const, volatile and restrict. */

#include <testfwk.h>

struct SAC
{
  char m[64];
};

struct SAP
{
  char *restrict m[64];
};

void
testBug (void)
{
  _Optional const struct SAC *pocs = 0;
  _Optional const volatile struct SAC *pocvs = 0;
  _Optional struct SAP *pos = 0;

  ASSERT (_Generic (pocs->m, const char *: 1, default: 0));
  ASSERT (_Generic (pocvs->m, const volatile char *: 1, default: 0));
  ASSERT (_Generic (pos->m, char *restrict *: 1, default: 0));
}
