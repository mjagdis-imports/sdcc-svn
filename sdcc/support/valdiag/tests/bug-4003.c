/* bug-4003.c

   Check arithmetic on pointers to _Optional even when zero offsets fold.
 */

/* Check the arithmetic diagnostic independently of other warnings. */
#pragma disable_warning 196

#ifdef TEST1
#pragma disable_warning 355
int *zero_plus (_Optional int *p)
{
  return 0 + p; /* WARNING */
}
#endif

#ifdef TEST2
#pragma disable_warning 355
int *plus_zero (_Optional int *p)
{
  return p + 0; /* WARNING */
}
#endif

#ifdef TEST3
#pragma disable_warning 355
int *known_nonnull (_Optional int *p)
{
  if (p)
    return p + 0;
  return 0;
}
#endif

#ifdef TEST4
#pragma disable_warning 355
int *known_object (void)
{
  static int object;
  _Optional int *p = &object;
  return 0 + p;
}
#endif

#ifdef TEST5
#pragma disable_warning 355
void unevaluated (_Optional int *p)
{
  (void)sizeof (p + 0);
  (void)sizeof (0 + p);
}
#endif

#ifdef TEST6
#pragma disable_warning 355
int *subscript_address (_Optional int *p)
{
  return &p[0]; /* WARNING */
}

int *checked_subscript_address (_Optional int *p)
{
  if (p)
    return &p[0];
  return 0;
}

void unevaluated_subscript_address (_Optional int *p)
{
  (void)sizeof (&p[0]);
}
#endif

#ifdef TEST7
#pragma disable_warning 355
int *minus_zero (_Optional int *p)
{
  return p - 0; /* WARNING */
}

int *checked_minus_zero (_Optional int *p)
{
  if (p)
    return p - 0;
  return 0;
}

void unevaluated_minus_zero (_Optional int *p)
{
  (void)sizeof (p - 0);
}
#endif

/* Keep non-null checks at the evaluated pointer operation. */

#ifdef TEST8
void discarded (_Optional int *p)
{
  (void)&*p; /* WARNING */
}
#endif

#ifdef TEST9
int read (_Optional int *p)
{
  return *p; /* WARNING */
}
#endif

#ifdef TEST10
void write (_Optional int *p)
{
  *p = 1; /* WARNING */
}
#endif

#ifdef TEST11
struct S { int member; };
int *member_address (_Optional struct S *p)
{
  return &p->member; /* WARNING */
}
#endif

#ifdef TEST12
typedef int Fn(void);
Fn *guarded_function (_Optional Fn *p)
{
  if (p)
    return *p;
  return 0;
}
#endif

#ifdef TEST13
typedef int Fn(void);
Fn *unguarded_function (_Optional Fn *p)
{
  return *p; /* WARNING */
}
#endif

#ifdef TEST14
int *later_guard (_Optional int *p)
{
  int *q = p + 0; /* WARNING */
  if (q)
    return q;
  return 0;
}
#endif

#ifdef TEST15
void discarded (_Optional int *p)
{
  (void)(p + 0); /* WARNING */
}
#endif

#ifdef TEST16
int difference (_Optional int *p)
{
  return p - p; /* WARNING */
}
#endif

#ifdef TEST17
int *constant_offset (_Optional int *p)
{
  int offset = 0;
  return p + offset; /* WARNING */
}
#endif

#ifdef TEST18
void increment (_Optional int *p)
{
  ++p; /* WARNING */
}
#endif

#ifdef TEST19
void decrement (_Optional int *p)
{
  p--; /* WARNING */
}
#endif

#ifdef TEST20
typedef int Fn(void);
int call (_Optional Fn *p)
{
  return p (); /* WARNING */
}
#endif

#ifdef TEST21
typedef int Fn(void);
int guarded_call (_Optional Fn *p)
{
  if (p)
    return p ();
  return 0;
}
#endif

#ifdef TEST22
int later_guard (_Optional int *p)
{
  int value = *p; /* WARNING */
  if (p)
    return value;
  return 0;
}
#endif

#ifdef TEST23
int difference (_Optional int *p, _Optional int *q)
{
  if (p)
    return p - q; /* WARNING */
  return 0;
}
#endif

#ifdef TEST24
int *null_address (void)
{
  return &*(_Optional int *)0; /* WARNING */
}
#endif
