/* Explicit constexpr integer initialisers must have integer constant type. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

void unrelated (void);

#ifndef SKIP_C23
#ifdef TEST1
constexpr int object = 1.0; /* ERROR(SDCC) */
#endif

#ifdef TEST2
constexpr int object = 1.5; /* ERROR(SDCC) */
#endif

#ifdef TEST3
struct S { int x; };
constexpr struct S object = {1.0}; /* ERROR(SDCC) */
#endif

#ifdef TEST4
constexpr int object[2] = {1, 2.0}; /* ERROR(SDCC) */
#endif

#ifdef TEST5
struct I { int x; };
struct S { struct I a[2]; };
constexpr struct S object = {{{1}, {2.0}}}; /* ERROR(SDCC) */
#endif

#ifdef TEST6
union U { int x; float y; };
constexpr union U object = {.x = 1.0}; /* ERROR(SDCC) */
#endif

#ifdef TEST7
/* Bit-field initialisation has its own emission path. */
struct S { unsigned int x : 3; };
constexpr struct S object = {1.0}; /* ERROR(SDCC) */
#endif

#ifdef TEST8
int test (void)
{
  constexpr int object = 1.0; /* ERROR(SDCC) */
  return object;
}
#endif

#ifdef TEST9
/* An explicit cast to integer produces an integer constant expression. */
constexpr int object = (int)1.5;
#endif

#ifdef TEST10
constexpr int object = 1 + 2;
#endif

#ifdef TEST11
constexpr float object = 1.0f;
#endif

#ifdef TEST12
/* Ordinary integer initialisation still permits floating conversion. */
int object = 1.5;
#endif

#ifdef TEST13
constexpr float source = 1.0f;
constexpr int object = source; /* ERROR(SDCC) */
#endif

#ifdef TEST14
struct S { int x; };
int test (void)
{
  constexpr struct S object = {1.0}; /* ERROR(SDCC) */
  return object.x;
}
#endif
#ifdef TEST15
/* Reading a constexpr must not change the source initialiser's type. */
constexpr int invalid = 1.0; /* ERROR(SDCC) */
constexpr int use = invalid + 0;
#endif
#endif
