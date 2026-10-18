/* Check all association pairs, including unselected compatible types. */
/* Keep skipped language-specific cases as nonempty translation units. */
typedef int bug_4134_translation_unit;

#if defined(__SDCC)
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#ifdef TEST1
/* The error must identify the later association, not the closing line. */
int object = _Generic(0,
  float: 1, /* IGNORE(GCC) */
  float: 2, /* ERROR */
  default: 0);
#endif

#ifdef TEST2
/* A typedef alias must identify its later unselected association too. */
typedef float Float;
int object = _Generic(0,
  float: 1, /* IGNORE(GCC) */
  Float: 2, /* ERROR */
  default: 0);
#endif

#ifdef TEST3
int object = _Generic(
  0,
  int *: 1, /* IGNORE(GCC) */
  int *: 2, /* ERROR */
  default: 0
);
#endif

#ifdef TEST4
/* Compatible array bounds must report the later association's line. */
int object = _Generic(0,
  int (*)[]: 1, /* IGNORE(GCC) */
  int (*)[2]: 2, /* ERROR */
  default: 0);
#endif

#ifdef TEST5
int object = _Generic(0, float: 1, char: 2, default: 0);
#endif

#ifdef TEST6
/* Distinct qualification means these association types are not compatible. */
int object = _Generic(0, float: 1, const float: 2, default: 0);
#endif

#ifdef TEST7
int object = _Generic(0, int (*)[2]: 1, int (*)[3]: 2, default: 0);
#endif

#ifdef TEST8
/* Matching duplicates must still be diagnosed. SDCC reports at the
   closing parenthesis; GCC reports at the later association. */
int object = _Generic(0,
  int: 1, /* IGNORE(GCC) */
  int: 2 /* ERROR(GCC) */
); /* ERROR(SDCC) */
#endif

#ifdef TEST9
int object = _Generic(
  0,
  default: 1, /* IGNORE(GCC) */
  default: 2 /* ERROR */
);
#endif

#ifdef TEST10
int object = _Generic(0, float: 1, int: 2, char: 3);
#endif

#ifdef TEST11
int object = _Generic(
  0, /* ERROR(GCC) */
  float: 1,
  char: 2 /* ERROR(SDCC) */
);
#endif

#ifdef TEST12
/* Bit-precise type qualifiers must also distinguish association types. */
#ifndef SKIP_C23
int object = _Generic(0, _BitInt(8): 1, const _BitInt(8): 2, default: 0);
#endif
#endif

#ifdef TEST13
#ifdef __SDCC
#pragma std_c2y
int object = _Generic(
  int,
  float: 1,
  float: 2, /* ERROR(SDCC) */
  default: 0
);
#endif
#endif

#ifdef TEST14
#ifdef __SDCC
#pragma std_c2y
int object = _Generic(int, float: 1, char: 2, default: 0);
#endif
#endif
