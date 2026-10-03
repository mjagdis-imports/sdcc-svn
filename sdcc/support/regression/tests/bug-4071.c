/* bug-4071.c
   Array-to-pointer conversion must preserve the address space
   through which a qualified array was reached.
 */

#include <testfwk.h>

struct S
{
  char m[64];
};

void
testBug (void)
{
  char (*pa)[64] = 0;
  struct S *ps = 0;

  ASSERT (_Generic (*pa, char *: 1, default: 0));
  ASSERT (_Generic (ps->m, char *: 1, default: 0));
}
