/* Guarded member loads must retain non-null inference through pointer casts. */
#if defined(__SDCC)
struct S
{
  _Optional int *p;
  _Optional int *q;
};

#ifdef TEST1
/* A guard on the member permits its dereference. */
int f(struct S *s)
{
  if (s->p)
    return *s->p;
  return 0;
}
#endif
#ifdef TEST2
/* Returning on null permits the later dereference. */
int f(struct S *s)
{
  if (!s->p)
    return 0;
  return *s->p;
}
#endif
#ifdef TEST3
/* The same guard works for a structure passed by value. */
int f(struct S s)
{
  if (s.p)
    return *s.p;
  return 0;
}
#endif
#ifdef TEST4
/* A const structure permits repeated dereferences after the guard. */
int f(const struct S *s)
{
  if (s->p)
    return *s->p + *s->p;
  return 0;
}
#endif
#ifdef TEST5
/* Assigning null invalidates the earlier guard. */
int f(struct S *s)
{
  if (s->p)
    {
      s->p = 0;
      return *s->p; /* WARNING(SDCC) */
    }
  return 0;
}
#endif
#ifdef TEST6
/* An indirect write may change the guarded member. */
int f(struct S *s, _Optional int **pp)
{
  if (s->p)
    {
      *pp = 0;
      return *s->p; /* WARNING(SDCC) */
    }
  return 0;
}
#endif
#ifdef TEST7
/* An unknown function may change the guarded member. */
void g(struct S *);
int f(struct S *s)
{
  if (s->p)
    {
      g(s);
      return *s->p; /* WARNING(SDCC) */
    }
  return 0;
}
#endif
#ifdef TEST8
/* The null branch does not permit dereference. */
int f(struct S *s)
{
  if (!s->p)
    return *s->p; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST9
/* Checking one member does not prove another is non-null. */
int f(struct S *s)
{
  if (s->p)
    return *s->q; /* WARNING(SDCC) */
  return 0;
}
#endif
#ifdef TEST10
/* A guard also permits dereference of a member at a nonzero offset. */
struct Later
{
  char padding;
  _Optional int *p;
};
int f(struct Later *s)
{
  if (s->p)
    return *s->p;
  return 0;
}
#endif
#ifdef TEST11
/* Writing null invalidates the guard at a nonzero offset too. */
struct Later
{
  char padding;
  _Optional int *p;
};
int f(struct Later *s)
{
  if (s->p)
    {
      s->p = 0;
      return *s->p; /* WARNING(SDCC) */
    }
  return 0;
}
#endif
#else
typedef int optional_member_requires_sdcc;
#endif
