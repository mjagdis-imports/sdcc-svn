/* bug-4044.c
   A compound literal without a storage class is not const.

   C11 6.5.2.5p4: the type of a compound literal is the type name written in
   it. Nothing adds a const there, so the object is modifiable and taking its
   address yields a pointer to non-const.

   SDCC infers constexpr for such a literal when the initializer allows it,
   which is a useful thing to know, but a declared constexpr is implicitly
   const (C23 6.7.2p6) and applying that to the inferred case changed the type
   the program sees. Two consequences: assigning the literal's address to a
   plain pointer warned that a const qualifier was lost (bug #4044), and on a
   target that puts const objects in ROM the object went there, so writing
   through the pointer did nothing.
 */

#include <testfwk.h>

struct A
{
	int i;
	int j;
};

struct A *fp = &(struct A){1, 2};

/* the shape from the bug report: compound literals nested through pointers */
struct B { struct A *a; struct A *b; };
struct C { struct B *c; struct A *d; };

struct C e = { &(struct B) { &(struct A) { 1, 2 },
                             &(struct A) { 3, 4 } },
               &(struct A) { 5, 6 } };

void
testFileScopeCompoundLiteralIsWritable (void)
{
      ASSERT (fp->i == 1);
      ASSERT (fp->j == 2);

      /* legal C: the literal is not const, so this must take effect */
      fp->i = 5;
      fp->j = 6;
      ASSERT (fp->i == 5);
      ASSERT (fp->j == 6);
}

void
testBlockScopeCompoundLiteralIsWritable (void)
{
      struct A *p = &(struct A){3, 4};

      ASSERT (p->i == 3);
      ASSERT (p->j == 4);

      p->i = 7;
      ASSERT (p->i == 7);
}

void
testNestedCompoundLiterals (void)
{
      ASSERT (e.c->a->i == 1);
      ASSERT (e.c->a->j == 2);
      ASSERT (e.c->b->i == 3);
      ASSERT (e.c->b->j == 4);
      ASSERT (e.d->i == 5);
      ASSERT (e.d->j == 6);
}
