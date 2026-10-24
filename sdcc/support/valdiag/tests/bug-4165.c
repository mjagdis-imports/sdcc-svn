/* Inherited _Optional must not qualify a pointer member's referenced type. */
#ifdef __SDCC
struct S
{
  int *a[2];
};

#ifdef TEST1
/* Without the fix, inherited _Optional qualifies the referenced int,
   producing warning 355 despite the check of s. */
int f(_Optional struct S *s)
{
  if (!s)
    return 0;
  return *s->a[0];
}
#endif

#ifdef TEST2
/* Without the fix, warning 355 also occurs when the array member and
   selected pointer element both have nonzero offsets. */
struct Padded
{
  char padding;
  int *a[2];
};
int f(_Optional struct Padded *s)
{
  if (!s)
    return 0;
  return *s->a[1];
}
#endif

#ifdef TEST3
/* Without the fix, inherited _Optional also reaches the referenced int
   through a multidimensional array, producing warning 355. */
struct Matrix
{
  int *a[2][3];
};
int f(_Optional struct Matrix *s)
{
  if (!s)
    return 0;
  return *s->a[1][2];
}
#endif

#ifdef TEST4
/* Without the fix, inherited _Optional reaches the referenced int for
   a union array member too, producing warning 355 after s is checked. */
union U
{
  int *a[2];
  int n;
};
int f(_Optional union U *s)
{
  if (!s)
    return 0;
  return *s->a[0];
}
#endif

#ifdef TEST5
/* Already passes without the fix: a member reached through a nested
   structure can be accessed after the outer pointer is checked. */
struct Outer
{
  char padding;
  struct S inner;
};
int f(_Optional struct Outer *s)
{
  if (!s)
    return 0;
  return *s->inner.a[1];
}
#endif

#ifdef TEST6
/* Without the fix, the referenced int gains _Optional, producing warning
   355 at the second indirection. Neither referenced type is _Optional-qualified
   in the member declaration. */
struct Deep
{
  int **a[2];
};
int f(_Optional struct Deep *s)
{
  if (!s)
    return 0;
  return **s->a[0];
}
#endif

#ifdef TEST7
/* Without the fix, inherited _Optional incorrectly qualifies int and
   produces warning 355. Const qualification of the structure still permits
   modification of the int referenced by its pointer element. */
void f(_Optional const struct S *s)
{
  if (!s)
    return;
  *s->a[0] = 1;
}
#endif

#ifdef TEST8
/* Without the fix, inherited _Optional incorrectly qualifies int and
   produces warning 355 when the structure is also volatile-qualified. */
int f(_Optional volatile struct S *s)
{
  if (!s)
    return 0;
  return *s->a[1];
}
#endif

#ifdef TEST9
/* Already passes without the fix: unchecked s points to an _Optional
   structure, so accessing its array member must produce warning 355. */
int f(_Optional struct S *s)
{
  int *p = s->a[0]; /* WARNING(SDCC) */
  return *p;
}
#endif

#ifdef TEST10
/* Already passes without the fix: checking s does not check s->a[0].
   The element points to _Optional int, so its unchecked dereference must
   still produce warning 355. */
struct PointersToOptionalInt
{
  _Optional int *a[2];
};
int f(_Optional struct PointersToOptionalInt *s)
{
  if (!s)
    return 0;
  return *s->a[0]; /* WARNING(SDCC) */
}
#endif

#ifdef TEST11
/* Already passes without the fix: access a restrict-qualified pointer
   member through a checked pointer to a const volatile structure.
   The referenced int can still be modified and is not _Optional-qualified. */
struct Direct
{
  int *restrict p;
};
int f(_Optional const volatile struct Direct *s)
{
  if (!s)
    return 0;
  *s->p = 1;
  return *s->p;
}
#endif

#ifdef TEST12
/* Already passes without the fix: read an integer array element through
   a checked pointer to a const volatile structure. */
struct Numbers
{
  int a[2];
};
int f(_Optional const volatile struct Numbers *s)
{
  if (!s)
    return 0;
  return s->a[1];
}
#endif

#else
/* Keep skipped SDCC-specific cases as nonempty translation units. */
typedef int bug_4165_requires_sdcc;
#endif
