/* bug-3393.c
   A bug in initalization of anonymous unions inside structs.
 */

#include <testfwk.h>

// Based on code sample by Howard M. Harte.

struct S {
	int a;
	union {
		struct {
			int i:4;
			int j:4;
		};
#if !defined(__SDCC) /* still fails because i,j & c are flattened into S */
		struct {
			int c:8;
		};
#endif
		int d;
	};
	int z;
};

struct S v = {1, 2, 3, 4};

int f (void)
{
      if (v.a != 1) return 1;
      if (v.i != 2) return 2;               /* holds 0: overwritten by init for d */
      if (v.j != 3) return 3;               /* holds 0: overwritten by init for d */
#if defined(__SDCC) /* only check for SDCC as it assumes integer size and endianness */
# if 0 /* still fails because i,j & c are flattened into S */
      if (v.c != 0x32) return 4;            /* holds 0: overwritten by init for d */
# endif

# if defined(__SDCC_stm8) || defined(__SDCC_hc08) || defined(__SDCC_s08)
      if ((v.d >> 8) != 0x32) return 5;     /* holds 0: none left */
# else
      if ((v.d & 0xFF) != 0x32) return 5;   /* holds 0: none left */
# endif
#endif
      if (v.z != 4) return 6;               /* holds 0: none left */
      return 0;
}

int g (void)
{
      struct S w = {5, 6, 7, 8};
      if (w.a != 5) return 1;
      if (w.i != 6) return 2;               /* holds 0: overwritten by init for d */
      if (w.j != 7) return 3;               /* holds 0: overwritten by init for d */
#if defined(__SDCC) /* only check for SDCC as it assumes integer size and endianness */
# if 0 /* still fails because i,j & c are flattened into S */
      if (w.c != 0x76) return 4;            /* holds 0: overwritten by init for d */
# endif

# if defined(__SDCC_stm8) || defined(__SDCC_hc08) || defined(__SDCC_s08)
      if ((w.d >> 8) != 0x76) return 5;     /* holds 0: none left */
# else
      if ((w.d & 0xFF) != 0x76) return 5;   /* holds 0: none left */
# endif
#endif
      if (w.z != 8) return 6;               /* holds 0: none left */
      return 0;
}

void
testBug (void)
{
      ASSERT (!f());
      ASSERT (!g());
}

