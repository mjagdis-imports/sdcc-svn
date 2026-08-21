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
		struct {
			int c:8;
		};
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
      if (v.c != 0x32) return 4;            /* holds 0: overwritten by init for d */

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
      if (w.c != 0x76) return 4;            /* holds 0: overwritten by init for d */

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

/* An alternative that is a struct larger than the first one. Its members
   past the first alternative carry offsets of their own, so neither the
   same-offset test nor the extent watermark can tell them from the fields
   that follow the union - only the mark made while the alternatives were
   still separate can. Built out of chars, so the expected values do not
   depend on endianness or padding and hold on the host port too. */
struct T {
	char a;
	union {
		char c;
		struct {
			char x;
			char y;
			char w;
		};
	};
	char z;
};

/* The first alternative is a struct too, so it consumes one initializer per
   member, and the members of the second one must consume none. */
struct U {
	char a;
	union {
		struct {
			char p;
			char q;
		};
		struct {
			char r;
			char s;
			char t;
		};
	};
	char z;
};

/* The union is the last member: the image has to be padded up to the size of
   the struct rather than up to a following field's offset. */
struct V {
	char a;
	union {
		char c;
		struct {
			char x;
			char y;
		};
	};
};

struct T gt = {1, 2, 3};
struct U gu = {1, 2, 3, 4};
struct V gv = {1, 2};

void
testBiggerAlternative (void)
{
      struct T v = {1, 2, 3};

      ASSERT (v.a == 1);
      ASSERT (v.c == 2);
      ASSERT (v.y == 0);        /* past the first alternative: zeroed */
      ASSERT (v.w == 0);
      ASSERT (v.z == 3);        /* held 0: y had consumed the 3 */

      ASSERT (gt.a == 1);
      ASSERT (gt.c == 2);
      ASSERT (gt.x == 2);
      ASSERT (gt.y == 0);       /* past the first alternative: zeroed */
      ASSERT (gt.w == 0);
      ASSERT (gt.z == 3);       /* held 0: the image was one byte short */
}

void
testFirstAlternativeStruct (void)
{
      struct U v = {1, 2, 3, 4};

      ASSERT (v.a == 1);
      ASSERT (v.p == 2);
      ASSERT (v.q == 3);        /* the first alternative consumes two */
      ASSERT (v.t == 0);        /* past the first alternative: zeroed */
      ASSERT (v.z == 4);        /* held 0: t had consumed the 4 */

      ASSERT (gu.a == 1);
      ASSERT (gu.p == 2);
      ASSERT (gu.q == 3);
      ASSERT (gu.r == 2);
      ASSERT (gu.s == 3);
      ASSERT (gu.t == 0);
      ASSERT (gu.z == 4);
}

void
testTrailingUnion (void)
{
      struct V v = {1, 2};

      ASSERT (v.a == 1);
      ASSERT (v.c == 2);
      ASSERT (v.y == 0);        /* past the first alternative: zeroed */

      ASSERT (gv.a == 1);
      ASSERT (gv.c == 2);
      ASSERT (gv.x == 2);
      ASSERT (gv.y == 0);       /* past the first alternative: zeroed */
}
