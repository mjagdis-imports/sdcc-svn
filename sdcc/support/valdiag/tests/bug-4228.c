/* Generic selections inspected as constexpr initialisers. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#if !defined(SKIP_C23)
#ifdef TEST1
constexpr int object = _Generic (1, int: 1, default: 1.0);
#endif

#ifdef TEST2
constexpr int object = _Generic (1, float: 1.0f, default: 2);
#endif

#ifdef TEST3
constexpr int object = _Generic (1, int: _Generic (1L, long: 3, default: 1.0), default: 2.0);
#endif

#ifdef TEST4
int nonconstant;
constexpr int object = _Generic (1, int: nonconstant, default: 1); /* ERROR */
#endif

#ifdef TEST5
constexpr int object = _Generic (1, float: 1.0f); /* ERROR */
#endif

#ifdef TEST6
void test(void)
{
  constexpr int object = _Generic (1, int: 6, default: 1.0);
  int array[object];
  _Static_assert(sizeof array == 6 * sizeof(int),
                 "array must have a constant bound");
}
#endif

#ifdef TEST7
constexpr unsigned char object = _Generic (1, int: 256, default: 0); /* WARNING(SDCC) */ /* ERROR(GCC) */
#endif

#ifdef TEST8
constexpr unsigned char object = _Generic (1, int: -1, default: 0); /* WARNING(SDCC) */ /* ERROR(GCC) */
#endif

#ifdef TEST9
constexpr signed char object = _Generic (1, int: _Generic (1L, long: 128, default: 0), default: 0); /* WARNING(SDCC) */ /* ERROR(GCC) */
#endif

#ifdef TEST10
constexpr unsigned char object = _Generic (1, float: 0.0f, default: 256); /* WARNING(SDCC) */ /* ERROR(GCC) */
#endif

#ifdef TEST11
/* An unselected out-of-range association does not affect initialisation. */
constexpr unsigned char object = _Generic (1, int: 255, default: 256);
#endif

#ifdef TEST12
constexpr unsigned char object = _Generic (1, float: 256, default: 0);
#endif

#ifdef TEST13
constexpr signed char object = _Generic (1, int: _Generic (1L, long: 127, default: 128), default: 128);
#endif

#ifdef TEST14
/* Binary32 can represent 2^24 exactly, but not 2^24 + 1. */
constexpr float object = _Generic (1, int: 16777216L, default: 16777217L);
#endif

#ifdef TEST15
constexpr float object = _Generic (1, float: 16777217L, default: 16777216L);
#endif

#ifdef TEST16
/* The explicit conversion precedes constexpr initialisation. */
constexpr float object = _Generic (1, int: (float)16777217L, default: 0);
#endif
#endif
