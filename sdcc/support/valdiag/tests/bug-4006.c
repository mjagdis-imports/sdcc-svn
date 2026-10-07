/* bug-4006.c

   Missing diagnostics when array-to-pointer conversion preserves a qualifier
   that is then discarded by an implicit conversion.
 */

#ifdef TEST1
/* Array decay preserves const qualification of the elements. */

struct SAC
{
  char m[64];
};

char *const_from_struct(const struct SAC *p)
{
  return p->m; // pointer target lost const qualifier /* WARNING */
}

char *const_from_array(const char (*p)[64])
{
  return *p; // pointer target lost const qualifier /* WARNING */
}

char *const_from_static(void)
{
  static const char s[10];
  return s; // pointer target lost const qualifier /* WARNING */
}

#endif

#ifdef TEST2
/* Distinguish declared and inherited volatile qualification from
   artificial union volatility. */

union V
{
  char a[4];
  volatile char va[4];
  struct
  {
    char a[4];
  } s;
};

char *union_array_member(union V *p)
{
  return p->a;
}

char *volatile_union_array_member(volatile union V *p)
{
  return p->a; /* WARNING */
}

char *explicitly_volatile_union_array_member(union V *p)
{
  return p->va; /* WARNING */
}

char *nested_union_member(union V *p)
{
  return p->s.a;
}

char *nested_volatile_union_member(volatile union V *p)
{
  return p->s.a; /* WARNING */
}

#endif

#ifdef TEST3
/* Array decay preserves volatile qualification of the elements. */

struct SAV
{
  char m[64];
};

char *volatile_from_struct(volatile struct SAV *p)
{
  return p->m; // pointer target lost volatile qualifier /* WARNING */
}

char *volatile_from_array(volatile char (*p)[64])
{
  return *p; // pointer target lost volatile qualifier /* WARNING */
}

char *volatile_from_static(void)
{
  static volatile char s[10];
  return s; // pointer target lost volatile qualifier /* WARNING */
}

#endif

#ifdef TEST4
/* Array decay preserves restrict and _Optional on pointer elements. */
struct SAO { _Optional char *m[64]; };
char **optional_from_struct(struct SAO *p)
{
  return p->m; /* WARNING */
}
char **optional_from_array(_Optional char *(*p)[64])
{
  return *p; /* WARNING */
}

struct SAR
{
  char *restrict m[64];
};

char **restrict_from_struct(struct SAR *p)
{
  return p->m; // pointer target lost restrict qualifier /* WARNING */
}

char **restrict_from_array(char *restrict (*p)[64])
{
  return *p; // pointer target lost restrict qualifier /* WARNING */
}

char **restrict_from_static(void)
{
  static char *restrict s[10];
  return s; // pointer target lost restrict qualifier /* WARNING */
}

#endif

#ifdef TEST5
/* Explicit casts may remove referenced qualifiers after array decay. */
char *explicit_const_cast(void)
{
  static const char a[2];
  return (char *)a;
}
char *explicit_volatile_cast(void)
{
  static volatile char a[2];
  return (char *)a;
}
char **explicit_optional_cast(void)
{
  static _Optional char *a[2];
  return (char **)a;
}
char **explicit_restrict_cast(void)
{
  static char *restrict a[2];
  return (char **)a;
}

#endif

#ifdef TEST6
/* Diagnose qualifier loss and dissimilar referenced types after decay
   in initialisers, assignments and returns. */
char **volatile_array_targets(void)
{
  static volatile char *a[2];
  char **p = a; /* WARNING */
  p = a; /* WARNING */
  return a; /* WARNING */
}
char **optional_array_targets(void)
{
  static _Optional char *a[2];
  char **p = a; /* WARNING */
  p = a; /* WARNING */
  return a; /* WARNING */
}
char **restrict_array_targets(void)
{
  static char *restrict a[2];
  char **p = a; /* WARNING */
  p = a; /* WARNING */
  return a; /* WARNING */
}
char **incompatible_array_targets(void)
{
  static const char *a[2];
  char **p = a; /* WARNING */
  p = a; /* WARNING */
  return a; /* WARNING */
}

#endif
