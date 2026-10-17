/* Pointer-valued compound literal initialisers are not constexpr objects. */
#include <testfwk.h>

struct S
{
  int *p;
};

union U
{
  int *p;
  int n;
};

static void
init (struct S *s, int *p)
{
  *s = (struct S){.p = p};
}

void
testPointerInitialisers (void)
{
  int a = 3;
  int b = 5;
  struct S s;
  int *p = &a;

  /* A parameter supplies the pointer at runtime, including on later calls. */
  init (&s, &a);
  ASSERT (s.p == &a);
  ASSERT (*s.p == 3);
  init (&s, &b);
  ASSERT (s.p == &b);
  ASSERT (*s.p == 5);

  /* Local pointers and union members use the same constant-expression check. */
  s = (struct S){p};
  ASSERT (s.p == &a);
  union U u;
  u = (union U){.p = p};
  ASSERT (u.p == &a);
  ASSERT (*u.p == 3);
}
