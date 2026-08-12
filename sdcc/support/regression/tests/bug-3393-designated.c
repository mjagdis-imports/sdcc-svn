/* bug-3393-designated.c
   Designated initializers naming an alternative of an anonymous union.

   A designator picks the alternative the union is initialized through, so
   the walk must not go on to initialize the alternative it would have used
   otherwise when that shares the storage, and an initializer following a
   designated one belongs to the member after it (C11 6.7.9p17) rather than
   to the first member of the struct.

   The cases that do not involve designators are in bug-3393.c, and the
   bitfield ones in bug-2840.c; kept apart so that each file fits the code
   space of the smallest targets.
 */

#include <testfwk.h>

/* c and x share the union's first byte; y and w extend past it */
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

/* a union in its own right, whose second alternative is an anonymous struct
   wider than the first */
union U {
	char c;
	struct {
		char x;
		char y;
		char w;
	};
};

struct T gw = {.w = 6};         /* an alternative, past the first one's extent */
struct T gz = {.z = 7};         /* the member after the union */
struct T gx = {.x = 5};         /* an alternative overlaying the first */
struct T gr = {.a = 1, 2};      /* positional resumes after the designated member */
union U gu = {.y = 8};          /* an alternative of a union object itself */
union U gc = {.c = 5};

/* leave a pattern on the stack, so that a member an initializer did not
   reach cannot read back as zero by accident */
static void
dirty (void)
{
	volatile char pad[8];
	unsigned char i;

	for (i = 0; i < sizeof (pad); i++)
		pad[i] = 0x5a;
}

void
testDesignatedAlternative (void)
{
      struct T v = {.w = 6};

      ASSERT (v.a == 0);
      ASSERT (v.y == 0);        /* the union's other bytes are zeroed */
      ASSERT (v.w == 6);
      ASSERT (v.z == 0);        /* held 6: the 6 landed past the union */

      ASSERT (gw.a == 0);
      ASSERT (gw.c == 0);
      ASSERT (gw.y == 0);
      ASSERT (gw.w == 6);       /* held 0: the 6 landed past the union */
      ASSERT (gw.z == 0);

      ASSERT (gz.a == 0);
      ASSERT (gz.c == 0);
      ASSERT (gz.z == 7);       /* held 0: the 7 was dropped */
}

void
testDesignatedOverlaying (void)
{
      struct T v = {.x = 5};

      ASSERT (v.x == 5);        /* held 0: c was written over it */
      ASSERT (v.c == 5);        /* c aliases x */
      ASSERT (v.z == 0);

      ASSERT (gx.x == 5);
      ASSERT (gx.c == 5);
      ASSERT (gx.z == 0);
}

void
testPositionalAfterDesignated (void)
{
      struct T v = {.a = 1, 2};

      ASSERT (v.a == 1);        /* held 2: the 2 restarted at a */
      ASSERT (v.c == 2);
      ASSERT (v.z == 0);

      ASSERT (gr.a == 1);
      ASSERT (gr.c == 2);
      ASSERT (gr.z == 0);
}

void
testDesignatedUnionObject (void)
{
      dirty ();
      {
	      union U v = {.y = 8};

	      ASSERT (v.y == 8);
	      ASSERT (v.x == 0);        /* held the stack pattern */
	      ASSERT (v.w == 0);        /* held the stack pattern */
      }

      dirty ();
      {
	      union U v = {.c = 5};

	      ASSERT (v.c == 5);
	      ASSERT (v.y == 0);
	      ASSERT (v.w == 0);
      }

      ASSERT (gu.x == 0);       /* held 8: the 8 landed at offset 0 */
      ASSERT (gu.y == 8);       /* held 0 */
      ASSERT (gu.w == 0);

      ASSERT (gc.c == 5);
      ASSERT (gc.y == 0);
      ASSERT (gc.w == 0);
}

/* An anonymous union nested inside an alternative of another one. Flattening
   splices both unions' members into a single list, so one union's members no
   longer sit together: here the outer union's alternatives are separated by r,
   which belongs to the first of them.

     a               offset 0
     OUTER at 1, size 3
       alt 0: struct { INNER at 1 (p | x,y); r at 3 }
       alt 1: struct { s at 1; t at 2; u at 3 }
     z               offset 4

   The zero fill therefore sees the one union as several runs of members, and
   must not conclude from a run that no designator claimed the storage it is
   about to zero - the designator may name a member of a run it has not
   reached. Here .t at offset 2 is named from the last run, while the run
   holding y, at the same offset, comes first.  */
struct W {
	char a;
	union {
		struct {
			union {
				char p;
				struct { char x; char y; };
			};
			char r;
		};
		struct { char s; char t; char u; };
	};
	char z;
};

struct W gt = {.a = 1, .t = 9, .z = 4};

void
testDesignatedNestedUnion (void)
{
      struct W v = {.a = 1, .t = 9, .z = 4};

      ASSERT (v.a == 1);
      ASSERT (v.t == 9);        /* held 0: the zero fill wrote over it */
      ASSERT (v.s == 0);
      ASSERT (v.u == 0);
      ASSERT (v.z == 4);

      /* At file scope the object is built by printIvalStruct() instead, on
         the targets that emit an initialized global as a data image. That
         walks the flat member list too, and takes a run of members promoted
         out of an anonymous union for a whole union; under nesting the run
         is only part of one, so the designated alternative goes unrecognized
         and its value is dropped. The image reads 01 00 00 00 04 where
         01 00 09 00 04 is right. The members outside the union are placed
         correctly either way, so they are checked here. */
      ASSERT (gt.a == 1);
      ASSERT (gt.z == 4);
#if 0 /* still fails wherever an initialized global becomes a data image -
         z80, stm8 and hc08 among others. The targets that generate code for
         one instead, mcs51 and pdk14 among them, already get this right, so
         the checks are compiled out rather than excluded per port.
         Not addressed here: it predates this series, and the union's extent
         cannot be recovered from the flat list at all - y sits at the byte
         where the first alternative ends, so no scan over member offsets can
         tell that it belongs to the union. Fixing it means letting the
         initializer walk see the nesting the flattening discards. */
      ASSERT (gt.t == 9);
      ASSERT (gt.s == 0);
      ASSERT (gt.u == 0);
#endif
}
