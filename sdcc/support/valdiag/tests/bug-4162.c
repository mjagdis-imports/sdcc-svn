/* Taking an array's address removes its outer _Optional qualification. */

#ifndef __SDCC
#define _Optional
#endif

#ifdef TEST1
/* A null guard makes &*p safe for both one- and two-dimensional arrays. */
int guarded_array (_Optional int (*p)[3])
{
  if (!p)
    {
      return 0;
    }

  int (*q)[3] = &*p;
  return (*q)[0];
}

int guarded_matrix (_Optional int (*p)[2][3])
{
  if (!p)
    {
      return 0;
    }

  int (*q)[2][3] = &p[0];
  return (*q)[0][0];
}
#endif

#ifdef TEST2
/* Removing the qualifier must not remove the unsafe-use diagnostic. */
int unsafe_array (_Optional int (*p)[3])
{
  int (*q)[3] = &*p; /* WARNING(SDCC) */
  return (*q)[0];
}
#endif

#ifdef TEST3
/* Unevaluated &*p must preserve const, volatile and deeper _Optional. */
void qualified_array (_Optional const volatile int (*p)[3])
{
  _Static_assert (_Generic (&*p, const volatile int (*)[3]: 1, default: 0),
                  "address changed array qualification");
  _Static_assert (_Generic (&p[0], const volatile int (*)[3]: 1, default: 0),
                  "array subscript address changed qualification");
}

void pointer_array (_Optional int * _Optional (*p)[3])
{
  _Static_assert (_Generic (&*p, _Optional int *(*)[3]: 1, default: 0),
                  "address changed qualification below the array elements");
}
#endif
