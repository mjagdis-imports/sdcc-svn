/* Each numbered block is a separate diagnostic check. */

#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

#ifdef TEST1
/* Decimal 2^64 exceeds unsigned long long; the baseline silently wraps. */
unsigned long long object = 18446744073709551616ULL; /* WARNING */
#endif

#ifdef TEST2
/* Hexadecimal 2^64 exceeds unsigned long long; the baseline is silent. */
unsigned long long object = 0x10000000000000000ULL; /* WARNING */
#endif

#ifdef TEST3
/* Octal 2^64 exceeds unsigned long long; the baseline is silent. */
unsigned long long object = 02000000000000000000000ULL; /* WARNING */
#endif

#ifdef TEST4
/* Several overflowing digits must produce one warning, not one per digit. */
unsigned long long object = 18446744073709551616000000000000000000000ULL; /* WARNING */
#endif

#ifdef TEST5
/* The same range violation also applies without a suffix. */
unsigned long long object = 18446744073709551616; /* WARNING */
#endif

#ifdef TEST6
/* Mixed-case suffixes must not hide an overflowing hexadecimal value. */
unsigned long long object = 0x10000000000000001uLL; /* WARNING */
#endif

#ifndef SKIP_C23

#ifdef TEST7
/* Binary 2^64 is invalid; C23 mode permits the binary spelling itself. */
unsigned long long object = 0b10000000000000000000000000000000000000000000000000000000000000000ULL; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif

#ifdef TEST8
/* Digit separators do not alter the range check; this is still 2^64. */
unsigned long long object = 18'446'744'073'709'551'616ULL; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif

#else
/* Keep a valid translation unit when the C23 cases are skipped. */
typedef int skipped_c23;
#endif

#ifdef TEST9
/* Valid case: the largest unsigned long long is accepted in decimal, hexadecimal and octal. */
unsigned long long decimal_max = 18446744073709551615ULL;
unsigned long long hex_max = 0xffffffffffffffffULL;
unsigned long long octal_max = 01777777777777777777777ULL;
#endif

#ifdef TEST10
/* Valid case: leading zeros, zero, and a large signed value remain representable. */
unsigned long long leading_zeros = 00000000000000000000000000000000000000000000000001ULL;
unsigned long long zero = 0ULL;
long long signed_max = 9223372036854775807LL;
#endif
