/* Use _Optional diagnostics to check volatile pointer reads. */

#ifdef __SDCC

#ifdef TEST1
/* Separate reads of volatile *pp cannot share a non-null inference. */
int f(_Optional int *volatile *pp)
{
  if (*pp)
    return *(*pp); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST2
/* Read volatile *pp once into non-volatile p, then check p. */
int f(_Optional int *volatile *pp)
{
  _Optional int *p = *pp;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST3
/* The two (*a)[0] expressions read the volatile first pointer in *a. */
int f(_Optional int *volatile (*a)[2])
{
  if ((*a)[0])
    return *((*a)[0]); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST4
/* Read the volatile first pointer in *a once into p, then check p. */
int f(_Optional int *volatile (*a)[2])
{
  _Optional int *p = (*a)[0];
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST5
/* The two (*a)[1] expressions read the volatile second pointer in *a. */
int f(_Optional int *volatile (*a)[2])
{
  if ((*a)[1])
    return *((*a)[1]); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST6
/* Read the volatile second pointer in *a once into p, then check p. */
int f(_Optional int *volatile (*a)[2])
{
  _Optional int *p = (*a)[1];
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST7
/* The two (*a)[i] expressions read a volatile pointer in the array *a. */
int f(_Optional int *volatile (*a)[2], unsigned char i)
{
  if ((*a)[i])
    return *((*a)[i]); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST8
/* Read the volatile pointer (*a)[i] once into p, then check p. */
int f(_Optional int *volatile (*a)[2], unsigned char i)
{
  _Optional int *p = (*a)[i];
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST9
/* The second read of s->inner.p is volatile, even with both offsets zero. */
struct Inner
{
  _Optional int *p;
};
struct Outer
{
  struct Inner inner;
};
int f(volatile struct Outer *s)
{
  if (s->inner.p)
    return *(s->inner.p); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST10
/* Read s->inner.p once into non-volatile p, then check p. */
struct Inner
{
  _Optional int *p;
};
struct Outer
{
  struct Inner inner;
};
int f(volatile struct Outer *s)
{
  _Optional int *p = s->inner.p;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST11
/* The second read of s->inner.p is volatile, with both offsets nonzero. */
struct Inner
{
  char pad;
  _Optional int *p;
};
struct Outer
{
  char pad;
  struct Inner inner;
};
int f(volatile struct Outer *s)
{
  if (s->inner.p)
    return *(s->inner.p); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST12
/* Read s->inner.p once into non-volatile p, then check p. */
struct Inner
{
  char pad;
  _Optional int *p;
};
struct Outer
{
  char pad;
  struct Inner inner;
};
int f(volatile struct Outer *s)
{
  _Optional int *p = s->inner.p;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST13
/* The second read of s->a[0] is volatile, even at offset zero. */
struct S
{
  _Optional int *a[2];
};
int f(volatile struct S *s)
{
  if (s->a[0])
    return *(s->a[0]); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST14
/* Read s->a[0] once into non-volatile p, then check p. */
struct S
{
  _Optional int *a[2];
};
int f(volatile struct S *s)
{
  _Optional int *p = s->a[0];
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST15
/* Read s->a[1] once into non-volatile p, then check p. */
struct S
{
  char pad;
  _Optional int *a[2];
};
int f(volatile struct S *s)
{
  _Optional int *p = s->a[1];
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST16
/* A check of volatile s->p does not constrain its second read. */
union U
{
  _Optional int *p;
  int n;
};
int f(volatile union U *s)
{
  if (s->p)
    return *(s->p); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST17
/* Read the volatile union member s->p once into p, then check p. */
union U
{
  _Optional int *p;
  int n;
};
int f(volatile union U *s)
{
  _Optional int *p = s->p;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST18
/* Volatility of the union applies to the nested pointer member s->s.p. */
struct S
{
  char pad;
  _Optional int *p;
};
union U
{
  struct S s;
  int n;
};
int f(volatile union U *s)
{
  if (s->s.p)
    return *(s->s.p); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST19
/* Read s->s.p once into non-volatile p, then check p. */
struct S
{
  char pad;
  _Optional int *p;
};
union U
{
  struct S s;
  int n;
};
int f(volatile union U *s)
{
  _Optional int *p = s->s.p;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST20
/* Conditional selection retains the volatile qualification of *a and *b. */
int f(_Optional int *volatile *a, _Optional int *volatile *b, int c)
{
  if (*(c ? a : b))
    return *(*(c ? a : b)); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST21
/* Copy the value of the selected volatile pointer into p, then check p. */
int f(_Optional int *volatile *a, _Optional int *volatile *b, int c)
{
  _Optional int *p = *(c ? a : b);
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST22
/* The comma expression still accesses the volatile pointer object *a. */
int f(_Optional int *volatile *a)
{
  if (*((void)0, a))
    return *(*((void)0, a)); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST23
/* Read *a through a comma expression once into p, then check p. */
int f(_Optional int *volatile *a)
{
  _Optional int *p = *((void)0, a);
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST24
/* The two *(a + 1) expressions read a volatile pointer through a. */
int f(_Optional int *volatile *a)
{
  if (*(a + 1))
    return *(*(a + 1)); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST25
/* Read the volatile pointer through a + 1 once into p, then check p. */
int f(_Optional int *volatile *a)
{
  _Optional int *p = *(a + 1);
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST26
/* Taking &s->p retains the member's volatile qualification. */
struct S
{
  char pad;
  _Optional int *p;
};
int f(volatile struct S *s)
{
  if (*(&s->p))
    return *(*(&s->p)); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST27
/* Read s->p through its address once into p, then check p. */
struct S
{
  char pad;
  _Optional int *p;
};
int f(volatile struct S *s)
{
  _Optional int *p = *(&s->p);
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST28
/* The const volatile pointer *pp may change between the two reads. */
int f(_Optional int *const volatile *pp)
{
  if (*pp)
    return *(*pp); /* WARNING(SDCC) */
  return 0;
}
#endif

#ifdef TEST29
/* Read the const volatile pointer *pp once into p, then check p. */
int f(_Optional int *const volatile *pp)
{
  _Optional int *p = *pp;
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST30
/* The two pointer reads in the logical AND are separate volatile accesses. */
int f(_Optional int *volatile *pp)
{
  return *pp && **pp; /* WARNING(SDCC) */
}
#endif

#ifdef TEST31
/* The condition and selected operand read the volatile pointer separately. */
int f(_Optional int *volatile *pp)
{
  return *pp ? **pp : 0; /* WARNING(SDCC) */
}
#endif

#ifdef TEST32
/* The volatile pointer may become null between the loop test and return. */
int f(_Optional int *volatile *pp)
{
  while (!*pp)
    {}
  return **pp; /* WARNING(SDCC) */
}
#endif

#ifdef TEST33
/* A volatile read after the assignment need not return the stored pointer. */
int f(_Optional int *volatile *pp, int *p)
{
  *pp = p;
  return **pp; /* WARNING(SDCC) */
}
#endif

#ifdef TEST34
/* The non-volatile nested pointer member s->inner.p retains its guard. */
struct Inner
{
  char pad;
  _Optional int *p;
};
struct Outer
{
  char pad;
  struct Inner inner;
};
int f(struct Outer *s)
{
  if (s->inner.p)
    return *(s->inner.p);
  return 0;
}
#endif

#ifdef TEST35
/* The checked non-volatile array element s->a[1] can be read again. */
struct S
{
  char pad;
  _Optional int *a[2];
};
int f(struct S *s)
{
  if (s->a[1])
    return *(s->a[1]);
  return 0;
}
#endif

#ifdef TEST36
/* Access through &s->p retains the non-volatile member's null check. */
struct S
{
  char pad;
  _Optional int *p;
};
int f(struct S *s)
{
  if (*(&s->p))
    return *(*(&s->p));
  return 0;
}
#endif

#ifdef TEST37
/* The const pointer *pp is not volatile, so its checked value can be reused. */
int f(_Optional int *const *pp)
{
  if (*pp)
    return *(*pp);
  return 0;
}
#endif

#ifdef TEST38
/* Both reads of s->a[1] are volatile through s (bug #4161). */
struct S
{
  char padding;
  _Optional int *a[2];
};

int f(volatile struct S *s)
{
  if (s->a[1])
    return *s->a[1]; /* WARNING(SDCC) */
  return 0;
}
#endif

#else
typedef int optional_volatile_requires_sdcc;
#endif
