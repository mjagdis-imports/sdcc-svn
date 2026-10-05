/* Unsigned bit-precise negation must wrap at the type's width. */
#pragma std_c2y
#ifdef TEST1
_Static_assert((unsigned long long)(-(unsigned _BitInt(8))1) == 255ULL, "negation");
_Static_assert((unsigned long long)(-(unsigned _BitInt(8))0) == 0, "zero");
#endif
#ifdef TEST2
_Static_assert((unsigned long)(-(unsigned _BitInt(64))1) == 4294967295UL, "low word");
_Static_assert((unsigned long)((unsigned long long)(-(unsigned _BitInt(64))1) >> 32) == 4294967295UL, "high word");
#endif
