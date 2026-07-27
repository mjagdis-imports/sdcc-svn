/* bug-3393.c
   A bug in initalization of anonymous unions inside structs.
 */

#include <testfwk.h>

// Based on code sample by Howard M. Harte.

struct S { int a; union { int c; int d; }; int z; };

int f (void)
{
      struct S v = {1, 2, 3};
      if (v.a != 1) return 1;
      if (v.c != 2) return 2;   /* holds 3: d aliased c */
      if (v.z != 3) return 3;   /* holds 0: none left   */
      return 0;
}

void
testBug (void)
{
#if 0 // Bug not yet fixed
      ASSERT (!f());
#endif
}

