/* Compile each selected constraint case separately. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif
int audit_nonempty;

#ifdef TEST1
/* Unsupported complex types require a diagnostic. */
float _Complex x; /* ERROR(SDCC) */
#endif

#ifdef TEST2
/* Unsupported decimal32 types require a diagnostic. */
_Decimal32 x; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif

#ifdef TEST3
/* Unsupported decimal64 types require a diagnostic. */
_Decimal64 x; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif

#ifdef TEST4
/* Unsupported decimal128 types require a diagnostic. */
_Decimal128 x; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif



#ifdef TEST7
#ifndef SKIP_C23
/* A constexpr object cannot have atomic type. */
constexpr _Atomic int x = 0; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST8
/* Valid case: a standard floating object is supported. */
float x;
#endif


#ifdef TEST10
#ifndef SKIP_C23
/* Valid case: constexpr has a non-atomic integer type. */
constexpr int x = 0;
#endif
#endif
