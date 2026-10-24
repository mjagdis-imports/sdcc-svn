/* Compile each selected constraint case separately. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif
#ifdef __SDCC
#define AUDIT_BITINT_MAXWIDTH __SDCC_BITINT_MAXWIDTH
#elif defined(__BITINT_MAXWIDTH__)
#define AUDIT_BITINT_MAXWIDTH __BITINT_MAXWIDTH__
#else
#define SKIP_BITINT
#endif
int audit_nonempty;

#ifdef TEST1
/* A function cannot return an array type. */
typedef int A[2];
A f(void); /* ERROR */
#endif


#ifdef TEST3
#ifndef SKIP_BITINT
/* A bit-precise width cannot exceed the implementation maximum. */
_BitInt(AUDIT_BITINT_MAXWIDTH + 1) x; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST4
/* An implicit integer-to-pointer conversion requires a diagnostic. */
int *f(int x)
{
  int *p = x; /* WARNING */
  return p;
}
#endif

#ifdef TEST5
/* An implicit pointer-to-integer conversion requires a diagnostic. */
int f(int *p)
{
  int x = p; /* WARNING */
  return x;
}
#endif

#ifdef TEST6
#ifndef SKIP_C23
/* A constexpr real floating object cannot have a pointer initialiser. */
constexpr float x = (int *)0; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST7
/* Valid case: a function may return a pointer to an array. */
typedef int A[2];
A *f(void);
#endif


#ifdef TEST9
#ifndef SKIP_BITINT
/* Valid case: the implementation maximum bit-precise width is allowed. */
_BitInt(AUDIT_BITINT_MAXWIDTH) x; /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST10
/* Valid case: an explicit integer-zero cast forms a null pointer. */
int *f(void)
{
  return (int *)0;
}
#endif

#ifdef TEST11
/* Valid case: a pointer-to-integer conversion is explicit. */
unsigned long f(int *p)
{
  return (unsigned long)p;
}
#endif

#ifdef TEST12
#ifndef SKIP_C23
/* Valid case: a constexpr real floating object has an integer initialiser. */
constexpr float x = 1;
#endif
#endif

#ifdef TEST13
/* An incomplete object pointer cannot be decremented by an integer. */
struct S;
struct S *f(struct S *p)
{
  return p - 0; /* ERROR */
}
#endif

#ifdef TEST14
/* Valid case: subtraction uses a pointer to a complete object. */
int *f(int *p)
{
  return p - 1;
}
#endif
