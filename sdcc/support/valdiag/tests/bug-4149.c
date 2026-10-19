/* Repeated volatile reads cannot share a non-null inference. */
#if defined(__SDCC)
struct First
{
  _Optional int *p;
};
struct Later
{
  char padding;
  _Optional int *p;
};

#ifdef TEST1
/* A second read of a volatile first member may yield null. */
int f(volatile struct First *s)
{
  if (s->p)
    return *s->p; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST2
/* A second read of a volatile later member may yield null too. */
int f(volatile struct Later *s)
{
  if (s->p)
    return *s->p; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST3
/* A single volatile read copied to a local pointer can be checked. */
int f(volatile struct First *s)
{
  _Optional int *p = s->p;
  if (p)
    return *p;
  return 0;
}
#endif
#ifdef TEST4
/* The same single-read workaround works for a later member. */
int f(volatile struct Later *s)
{
  _Optional int *p = s->p;
  if (p)
    return *p;
  return 0;
}
#endif
#ifdef TEST5
/* Direct qualification also makes the first pointer member volatile. */
struct V
{
  _Optional int *volatile p;
};
int f(struct V *s)
{
  if (s->p)
    return *s->p; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST6
/* Direct qualification also makes a later pointer member volatile. */
struct V
{
  char padding;
  _Optional int *volatile p;
};
int f(struct V *s)
{
  if (s->p)
    return *s->p; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST7
/* A volatile pointed-to int does not make the pointer member volatile. */
struct V
{
  _Optional volatile int *p;
};
int f(struct V *s)
{
  _Optional volatile int *p = s->p;
  if (p)
    return *p;
  return 0;
}
#endif
#else
typedef int optional_volatile_member_requires_sdcc;
#endif
