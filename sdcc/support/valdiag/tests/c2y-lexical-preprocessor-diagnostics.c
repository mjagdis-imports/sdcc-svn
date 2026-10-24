/* Compile each selected case separately. */
#ifdef __SDCC
#pragma std_c23
#endif
int audit_nonempty;

#ifdef TEST1
/* A stray character cannot form a C token. */
int x = @; /* ERROR */
#endif

#ifdef TEST2
/* A malformed preprocessing number cannot form an integer literal. */
int x = 0xg; /* ERROR */
#endif

#ifdef TEST3
/* has_include requires a header-name token sequence. */
#if __has_include(123) /* ERROR */
#endif
#endif

#ifdef TEST4
/* has_c_attribute requires the form of an attribute token. */
#if __has_c_attribute(123) /* ERROR */
#endif
#endif

#ifdef TEST5
/* A stray token cannot occur in a preprocessing controlling expression. */
#if @ /* ERROR */
#endif
#endif

#ifdef TEST6
/* Floating literals are not permitted in preprocessing controlling expressions. */
#if 1.0 /* ERROR */
#endif
#endif

#ifdef TEST7
/* An include directive must identify an available source or header. */
#include "sdcc-audit-missing-5f4c.h" /* ERROR */
#endif

#ifdef TEST8
/* Valid case: a non-basic universal character can occur in a string. */
const char *p = "\u03b1";
#endif

#ifdef TEST9
/* Valid case: header lookup finds this file relative to its directory. */
#if !__has_include("c2y-lexical-preprocessor-diagnostics.c")
#error source file not found
#endif
#endif

#ifdef TEST10
/* Valid case: preprocessing arithmetic uses integer operands. */
#if 1 + 2 != 3
#error incorrect preprocessing arithmetic
#endif
#endif

#ifdef TEST11
/* Valid case: an ordinary identifier and integer literal form valid tokens. */
int value = 1;
#endif
