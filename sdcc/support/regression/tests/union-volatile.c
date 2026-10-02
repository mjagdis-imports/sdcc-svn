/*
   Bug #4072 - artificial union volatility must not change the C member type.
*/
#include <testfwk.h>

#ifdef __SDCC
#pragma std_c11
#endif

struct S { char a[2]; };
union U
{
  char a[4];
  char *p;
  struct S s;
};
static union U u;
static volatile union U vu;

void
testWrites (void)
{
  char first = 11, second = 22;

  u.s.a[0] = 11;
  u.a[0] = 22;
  ASSERT (u.s.a[0] == 22);
  u.p = &first;
  ASSERT (*u.p == 11);
  u.p = &second;
  ASSERT (*u.p == 22);
}

void
testMemberTypes (void)
{
#ifdef __SDCC
  union U *p = &u;
  volatile union U *vp = &vu;

  _Static_assert (_Generic (p->a, char *: 1, default: 0), "array member");
  _Static_assert (_Generic (vp->a, volatile char *: 1, default: 0), "volatile array member");
  _Static_assert (_Generic (p->p, char *: 1, default: 0), "pointer member");

  /* testWrites is compiled first; its iCode qualifiers must not change u.a. */
  _Static_assert (_Generic (&p->a, char (*)[4]: 1, default: 0), "shared member type");
#endif
}
