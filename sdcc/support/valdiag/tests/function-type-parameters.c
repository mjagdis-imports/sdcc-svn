/* Check function parameters in exact comparisons. */
#ifdef TEST1
typedef int F(int); /* ERROR */
typedef int F(float); /* ERROR */
#endif

#ifdef TEST2
typedef int F(int);
typedef int F(const int);
#endif

#ifdef TEST3
typedef int FI(int);
typedef int FF(float);
void test(FI *(*a)(void), FF *(*b)(void))
{
  a = b; /* WARNING */
}
#endif

#ifdef TEST4
/* Ignore const on a scalar parameter, but preserve atomicity. */
typedef void Atomic(_Atomic int); /* ERROR */
typedef void Plain(int);
typedef void Const(const int);
typedef Atomic *PA;
typedef Plain *PP;
typedef Const *PC;
_Static_assert(_Generic((PA *)0, PP *: 0, default: 1), "parameter type");
_Static_assert(_Generic((PP *)0, PA *: 0, default: 1), "parameter type");
_Static_assert(_Generic((PP *)0, PC *: 1, default: 0), "parameter type");
#endif

#ifdef TEST5
/* Pointer parameter atomicity differs from access qualification. */
typedef int *Pointer;
typedef void Atomic(_Atomic Pointer); /* ERROR */
typedef void Plain(Pointer);
typedef void Const(const Pointer);
typedef Atomic *PA;
typedef Plain *PP;
typedef Const *PC;
_Static_assert(_Generic((PA *)0, PP *: 0, default: 1), "parameter type");
_Static_assert(_Generic((PP *)0, PA *: 0, default: 1), "parameter type");
_Static_assert(_Generic((PP *)0, PC *: 1, default: 0), "parameter type");
#endif

#ifdef TEST6
/* Removing access qualification must not remove atomicity. */
typedef void Atomic(_Atomic int); /* ERROR */
typedef void ConstAtomic(const _Atomic int); /* ERROR */
typedef Atomic *PA;
typedef ConstAtomic *PCA;
_Static_assert(_Generic((PA *)0, PCA *: 1, default: 0), "parameter type");
#endif

#ifdef TEST7
/* Function-pointer conversions compare scalar parameter atomicity. */
typedef void Atomic(_Atomic int); /* ERROR */
typedef void Plain(int);
void test(Atomic *a, Plain *p)
{
  a = p; /* WARNING */
  p = a; /* WARNING */
}
#endif

#ifdef TEST8
/* Function-pointer conversions compare pointer parameter atomicity. */
typedef int *Pointer;
typedef void Atomic(_Atomic Pointer); /* ERROR */
typedef void Plain(Pointer);
void test(Atomic *a, Plain *p)
{
  a = p; /* WARNING */
  p = a; /* WARNING */
}
#endif

#ifdef TEST9
/* Matching atomic pointers may differ in access qualification. */
typedef int *Pointer;
typedef void Atomic(_Atomic Pointer); /* ERROR */
typedef void ConstAtomic(const _Atomic Pointer); /* ERROR */
typedef Atomic *PA;
typedef ConstAtomic *PCA;
_Static_assert(_Generic((PA *)0, PCA *: 1, default: 0), "parameter type");
#endif
