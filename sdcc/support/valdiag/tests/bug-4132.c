/* An atomic type specifier must diagnose invalid derived types safely. */
#ifdef SDCC
#pragma std_c23
#endif

#ifdef TEST1
typedef int Array[2];
_Atomic(Array) object; /* ERROR */
#endif

#ifdef TEST2
typedef int Function(void);
_Atomic(Function) function; /* ERROR */
#endif

#ifdef TEST3
typedef const int Array[2];
_Atomic(Array) object; /* ERROR */
#endif

#ifdef TEST4
/* Atomic pointers are valid types, but atomics are unsupported by SDCC. */
_Atomic(int *) object; /* ERROR(SDCC) */
#endif

#ifdef TEST5
typedef int *const Pointer;
_Atomic(Pointer) object; /* ERROR */
#endif

#ifdef TEST6
_Atomic(const int) object; /* ERROR */
#endif

#ifdef TEST7
typedef _Atomic(int) Atomic; /* ERROR(SDCC) */
_Atomic(Atomic) object; /* ERROR */
#endif

#ifdef TEST8
/* Valid cases: ordinary arrays, functions and function pointers. */
typedef int Array[2];
typedef int Function(void);
Array object;
Function function;
Function *pointer;
#endif

#ifdef TEST9
/* Atomic function pointers also need the unsupported-atomic diagnostic. */
typedef int (*Pointer)(void);
_Atomic(Pointer) object; /* ERROR(SDCC) */
#endif
