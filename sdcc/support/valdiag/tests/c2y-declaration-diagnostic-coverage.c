/* Compile each selected constraint case separately. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif
int audit_nonempty;

#ifdef TEST1
#ifndef SKIP_C23
/* An extern block-scope compound literal cannot have an initialiser. */
void f(void)
{
  (void)(extern int){1}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif
#endif

#ifdef TEST2
#ifndef SKIP_C23
/* Static and register cannot be combined. */
void f(void)
{
  (void)(static register int){1}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif
#endif

#ifdef TEST3
#ifndef SKIP_C23
/* A static compound literal needs a constant initialiser. */
void f(int n)
{
  (void)(static int){n}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif
#endif

#ifdef TEST4
/* A variably modified typedef name cannot have file scope. */
int n;
typedef int A[n]; /* ERROR */
#endif


#ifdef TEST5
/* A static object cannot have variable length array type. */
void f(int n)
{
  static int a[n]; /* ERROR */
}
#endif


#ifdef TEST6
/* A VLA cannot have a nonempty initialiser. */
void f(int n)
{
  int a[n] = {0}; /* ERROR */
}
#endif



#ifdef TEST8
#ifndef SKIP_C23
/* Valid case: a static compound literal has a constant initialiser. */
int *f(void)
{
  return &(static int){1};
}
#endif
#endif

#ifdef TEST9
#ifndef SKIP_C23
/* Valid case: an ordinary block-scope pointer has no linkage. */
void f(int n)
{
  int (*p)[n] = 0;
  (void)p;
}
#endif
#endif
