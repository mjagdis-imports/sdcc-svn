/* bug-2840.c
   Bitfield alternatives of an anonymous union inside a struct.

   promoteAnonStructs() flattens an anonymous union's members into the
   enclosing struct. The union is initialized through its first named member
   only (C11 6.7.9p17), so the members promoted out of the other alternatives
   consume no initializer - but bitfields sharing one storage unit share a
   byte offset with each other as well, so byte offsets cannot tell the two
   apart, and an unnamed bitfield is not an alternative that can be
   initialized at all.

   This is bug #2840, the bitfield side of bug #3393. The cases made of whole
   bytes are in bug-3393.c, which also covers a union whose alternatives are
   anonymous structs of bitfields.

   Kept separate from bug-3393.c so that both files fit the code space of the
   smallest targets.
 */

#include <testfwk.h>

/* An unnamed bitfield ahead of the first named alternative. i is still the
   member the union is initialized through; the walk would otherwise take the
   unnamed bitfield for the alternative to emit and then mistake i for an
   alias of it. */
struct S {
	char a;
	union {
		int : 3;
		int i : 4;
		int c : 8;
	};
	char z;
};

/* The same with the unnamed bitfield after the first named alternative. */
struct T {
	char a;
	union {
		int i : 4;
		int : 3;
		int c : 8;
	};
	char z;
};

struct S gs = {1, 2, 3};
struct T gt = {1, 2, 3};

void
testUnnamedFirst (void)
{
      struct S v = {1, 2, 3};

      ASSERT (v.a == 1);
      ASSERT (v.i == 2);
      ASSERT (v.z == 3);        /* held 0: i was taken for an alias */

      ASSERT (gs.a == 1);
      ASSERT (gs.i == 2);
      ASSERT (gs.z == 3);       /* held 0: the image lost i's initializer */
}

void
testUnnamedAfter (void)
{
      struct T v = {1, 2, 3};

      ASSERT (v.a == 1);
      ASSERT (v.i == 2);
      ASSERT (v.z == 3);

      ASSERT (gt.a == 1);
      ASSERT (gt.i == 2);
      ASSERT (gt.z == 3);
}
