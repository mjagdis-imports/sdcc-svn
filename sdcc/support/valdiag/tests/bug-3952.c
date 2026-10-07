/* bug-3952.c

   Diagnose implicit conversions between pointers to incompatible types.
 */

void takes_float(float *);

#ifdef TEST1
float *itof(int *i)
{
  return i; /* WARNING */
}

void reported_cases(int *i)
{
  float *f = itof(i);
  float object;
  int *p = (int *)&object;
  float *d = (int *)&object; /* WARNING */

  /* The report's i = itof(f) contains two violations.  Keep them on
     separate lines so valdiag verifies both diagnostics independently. */
  i = f; /* WARNING */
  (void)itof(f); /* WARNING */
  takes_float(i); /* WARNING */

  (void)p;
  (void)d;
}
#endif

#ifdef TEST2
const int *incompatible_return_declarations(void); /* IGNORE */
volatile int *incompatible_return_declarations(void) /* ERROR */
{
  return 0;
}
#endif

#ifdef TEST3
struct A { int x; };
struct B { int x; };

void incompatible_structs(struct A *a, struct B *b)
{
  a = b; /* WARNING */
}
#endif

#ifdef TEST4
void incompatible_character_types(char *p, signed char *s, unsigned char *u)
{
  p = s; /* WARNING */
  p = u; /* WARNING */
  s = u; /* WARNING */
}
#endif

#ifdef TEST5
void incompatible_nested_pointers(int **p, float **f)
{
  p = f; /* WARNING */
}
#endif

#ifdef TEST6
void compatible_object_pointers(int *p, const int *cp, volatile int *vp,
                                void *v, const void *cv)
{
  cp = p;
  vp = p;
  v = p;
  cv = cp;
  cp = cv;
  p = (int *)(float *)0;
}
#endif

#ifdef TEST7
typedef int FI(int);
typedef int FF(float);

void incompatible_function_pointers(FI *pi, FF *pf)
{
  pi = pf; /* WARNING */
}
#endif

#ifdef TEST8
int incompatible_array[2];

float *returns_array(void)
{
  return incompatible_array; /* WARNING */
}

void incompatible_array_conversions(void)
{
  float *p = incompatible_array; /* WARNING */
  p = incompatible_array; /* WARNING */
  takes_float(incompatible_array); /* WARNING */
  (void)p;
}
#endif

#ifdef TEST9
static int static_object;
int array_object[2];

int *compatible_returns(void)
{
  return &static_object;
}

void compatible_storage(void)
{
  int *p = &static_object;
  p = array_object;
  (void)p;
}

void compatible_array_bounds(int (*known)[2], int (*unknown)[],
                             int (**nested_known)[2], int (**nested_unknown)[])
{
  known = unknown;
  unknown = known;
  nested_known = nested_unknown;
  nested_unknown = nested_known;
}
#endif

#ifdef TEST10
void incompatible_pointer_returns(const int *(*pc)(void), volatile int *(*pv)(void))
{
  pc = pv; /* WARNING */
}
#endif

#ifdef TEST11
void incompatible_char_returns(char (*plain)(void), signed char (*explicit_signed)(void))
{
  plain = explicit_signed; /* WARNING */
}
#endif

#ifdef TEST12
struct ReturnA { int x; };
struct ReturnB { int x; };
void incompatible_struct_returns(struct ReturnA (*a)(void), struct ReturnB (*b)(void))
{
  a = b; /* WARNING */
}
#endif

#ifdef TEST13
typedef const int QualifiedReturn(void); /* WARNING */
void compatible_qualified_returns(int (*p)(void), QualifiedReturn *cp,
                                  const int *(*pc)(void), const int *(*other_pc)(void))
{
  cp = p;
  p = cp;
  pc = other_pc;
}
#endif

#ifdef TEST14
#if defined(__SDCC_z80) || defined(__SDCC_z180)
void incompatible_calling_conventions(int (*a)(void) __sdcccall(0),
                                     int (*b)(void) __sdcccall(1))
{
  a = b; /* WARNING(SDCC_z80 || SDCC_z180) */
}
#endif
#endif

#ifdef TEST15
/* Atomic referenced types remain distinct, even though SDCC cannot allocate
   the atomic parameter objects used by its parameter-passing implementation. */
void atomic_referenced_types(_Atomic int *atomic, int *plain, /* ERROR */
                             const _Atomic int *qualified) /* ERROR */
{
  qualified = atomic;
  atomic = plain; /* WARNING */
  plain = atomic; /* WARNING */
}
#endif

#ifdef TEST16
void similar_pointer_referenced_types(int **plain, int *const *qualified,
                                      void *v)
{
  qualified = plain;
  v = plain;
  plain = v;
}
#endif

#ifdef TEST17
const int normalised_return(void); /* WARNING */
volatile int normalised_return(void)
{ /* WARNING */
  return 0;
}
#endif

#ifdef TEST18
int *const normalised_pointer_return(void); /* WARNING */
int *normalised_pointer_return(void)
{
  return 0;
}
#endif

#ifdef TEST19
void normalised_abstract_return(void)
{
  int (**p)(void) = (const int (**)(void))0; /* WARNING */
  (void)p;
}
#endif

#ifdef TEST20
#pragma disable_warning 361
typedef _Atomic int AtomicReturn(void); /* ERROR */
typedef int PlainReturn(void);
typedef AtomicReturn *AtomicReturnPointer;
typedef PlainReturn *PlainReturnPointer;
_Static_assert(_Generic((AtomicReturnPointer *)0,
                        PlainReturnPointer *: 1, default: 0), "return type");
#endif
