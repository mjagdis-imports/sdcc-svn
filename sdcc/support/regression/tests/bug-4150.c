/* A for declaration must be visible to type queries in its header. */
#include <testfwk.h>

static inline int *
next (int *p)
{
  return p + 1;
}

void
testForScope (void)
{
  int a[3] = {0, 0, 0};
  int p = 7;
  int count = 0;

  /* &* queries the loop variable's type before the body is parsed. */
  for (int *p = a; &*p != a + 3; p = next (&*p))
    {
      *p = ++count;
    }
  ASSERT (count == 3);
  ASSERT (a[0] == 1);
  ASSERT (a[1] == 2);
  ASSERT (a[2] == 3);
  ASSERT (p == 7);

  /* The same scope rule applies without an inline call. */
  count = 0;
  for (int *p = a; p != a + 3; p = &*p + 1)
    {
      count += *p;
    }
  ASSERT (count == 6);
}
