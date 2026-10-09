/* Minimum/-1 folding must not crash or retain an incorrectly signed quotient.
   Warning 165 is disabled so this also tests a later diagnostic patch. */

#ifdef __SDCC
#pragma disable_warning 165
#define CHECK_VALUE(expression, expected) \
  _Static_assert((unsigned long)(unsigned long long)(expression) == \
                 (unsigned long)(unsigned long long)(expected), "low word"); \
  _Static_assert((unsigned long)((unsigned long long)(expression) >> 32) == \
                 (unsigned long)((unsigned long long)(expected) >> 32), "high word")
#endif

#ifdef TEST1
#ifdef __SDCC
/* int: overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((int)(-32767LL - 1LL) / (int)-1), (int)(-32767LL - 1LL));
_Static_assert(((int)(-32767LL - 1LL) / (int)-1) < 0, "negative quotient");
CHECK_VALUE(((int)(-32767LL - 1LL) % (int)-1), 0);
CHECK_VALUE(((int)(-32767LL - 1LL) / (int)1), (int)(-32767LL - 1LL));
CHECK_VALUE(((int)(-32767LL) / (int)-1), (int)32767LL);
#endif
#endif

#ifdef TEST2
#ifdef __SDCC
/* long: overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((long)(-2147483647LL - 1LL) / (long)-1), (long)(-2147483647LL - 1LL));
_Static_assert(((long)(-2147483647LL - 1LL) / (long)-1) < 0, "negative quotient");
CHECK_VALUE(((long)(-2147483647LL - 1LL) % (long)-1), 0);
CHECK_VALUE(((long)(-2147483647LL - 1LL) / (long)1), (long)(-2147483647LL - 1LL));
CHECK_VALUE(((long)(-2147483647LL) / (long)-1), (long)2147483647LL);
#endif
#endif

#ifdef TEST3
#ifdef __SDCC
/* long long: overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((long long)(-9223372036854775807LL - 1LL) / (long long)-1), (long long)(-9223372036854775807LL - 1LL));
_Static_assert(((long long)(-9223372036854775807LL - 1LL) / (long long)-1) < 0, "negative quotient");
CHECK_VALUE(((long long)(-9223372036854775807LL - 1LL) % (long long)-1), 0);
CHECK_VALUE(((long long)(-9223372036854775807LL - 1LL) / (long long)1), (long long)(-9223372036854775807LL - 1LL));
CHECK_VALUE(((long long)(-9223372036854775807LL) / (long long)-1), (long long)9223372036854775807LL);
#endif
#endif

#ifdef TEST4
#ifdef __SDCC
/* _BitInt(2): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(2))(-1LL - 1LL) / (_BitInt(2))-1), (_BitInt(2))(-1LL - 1LL));
_Static_assert(((_BitInt(2))(-1LL - 1LL) / (_BitInt(2))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(2))(-1LL - 1LL) % (_BitInt(2))-1), 0);
CHECK_VALUE(((_BitInt(2))(-1LL - 1LL) / (_BitInt(2))1), (_BitInt(2))(-1LL - 1LL));
CHECK_VALUE(((_BitInt(2))(-1LL) / (_BitInt(2))-1), (_BitInt(2))1LL);
#endif
#endif

#ifdef TEST5
#ifdef __SDCC
/* _BitInt(8): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(8))(-127LL - 1LL) / (_BitInt(8))-1), (_BitInt(8))(-127LL - 1LL));
_Static_assert(((_BitInt(8))(-127LL - 1LL) / (_BitInt(8))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(8))(-127LL - 1LL) % (_BitInt(8))-1), 0);
CHECK_VALUE(((_BitInt(8))(-127LL - 1LL) / (_BitInt(8))1), (_BitInt(8))(-127LL - 1LL));
CHECK_VALUE(((_BitInt(8))(-127LL) / (_BitInt(8))-1), (_BitInt(8))127LL);
#endif
#endif

#ifdef TEST6
#ifdef __SDCC
/* _BitInt(16): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(16))(-32767LL - 1LL) / (_BitInt(16))-1), (_BitInt(16))(-32767LL - 1LL));
_Static_assert(((_BitInt(16))(-32767LL - 1LL) / (_BitInt(16))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(16))(-32767LL - 1LL) % (_BitInt(16))-1), 0);
CHECK_VALUE(((_BitInt(16))(-32767LL - 1LL) / (_BitInt(16))1), (_BitInt(16))(-32767LL - 1LL));
CHECK_VALUE(((_BitInt(16))(-32767LL) / (_BitInt(16))-1), (_BitInt(16))32767LL);
#endif
#endif

#ifdef TEST7
#ifdef __SDCC
/* _BitInt(17): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(17))(-65535LL - 1LL) / (_BitInt(17))-1), (_BitInt(17))(-65535LL - 1LL));
_Static_assert(((_BitInt(17))(-65535LL - 1LL) / (_BitInt(17))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(17))(-65535LL - 1LL) % (_BitInt(17))-1), 0);
CHECK_VALUE(((_BitInt(17))(-65535LL - 1LL) / (_BitInt(17))1), (_BitInt(17))(-65535LL - 1LL));
CHECK_VALUE(((_BitInt(17))(-65535LL) / (_BitInt(17))-1), (_BitInt(17))65535LL);
#endif
#endif

#ifdef TEST8
#ifdef __SDCC
/* _BitInt(24): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(24))(-8388607LL - 1LL) / (_BitInt(24))-1), (_BitInt(24))(-8388607LL - 1LL));
_Static_assert(((_BitInt(24))(-8388607LL - 1LL) / (_BitInt(24))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(24))(-8388607LL - 1LL) % (_BitInt(24))-1), 0);
CHECK_VALUE(((_BitInt(24))(-8388607LL - 1LL) / (_BitInt(24))1), (_BitInt(24))(-8388607LL - 1LL));
CHECK_VALUE(((_BitInt(24))(-8388607LL) / (_BitInt(24))-1), (_BitInt(24))8388607LL);
#endif
#endif

#ifdef TEST9
#ifdef __SDCC
/* _BitInt(32): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(32))(-2147483647LL - 1LL) / (_BitInt(32))-1), (_BitInt(32))(-2147483647LL - 1LL));
_Static_assert(((_BitInt(32))(-2147483647LL - 1LL) / (_BitInt(32))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(32))(-2147483647LL - 1LL) % (_BitInt(32))-1), 0);
CHECK_VALUE(((_BitInt(32))(-2147483647LL - 1LL) / (_BitInt(32))1), (_BitInt(32))(-2147483647LL - 1LL));
CHECK_VALUE(((_BitInt(32))(-2147483647LL) / (_BitInt(32))-1), (_BitInt(32))2147483647LL);
#endif
#endif

#ifdef TEST10
#ifdef __SDCC
/* _BitInt(33): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(33))(-4294967295LL - 1LL) / (_BitInt(33))-1), (_BitInt(33))(-4294967295LL - 1LL));
_Static_assert(((_BitInt(33))(-4294967295LL - 1LL) / (_BitInt(33))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(33))(-4294967295LL - 1LL) % (_BitInt(33))-1), 0);
CHECK_VALUE(((_BitInt(33))(-4294967295LL - 1LL) / (_BitInt(33))1), (_BitInt(33))(-4294967295LL - 1LL));
CHECK_VALUE(((_BitInt(33))(-4294967295LL) / (_BitInt(33))-1), (_BitInt(33))4294967295LL);
#endif
#endif

#ifdef TEST11
#ifdef __SDCC
/* _BitInt(63): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(63))(-4611686018427387903LL - 1LL) / (_BitInt(63))-1), (_BitInt(63))(-4611686018427387903LL - 1LL));
_Static_assert(((_BitInt(63))(-4611686018427387903LL - 1LL) / (_BitInt(63))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(63))(-4611686018427387903LL - 1LL) % (_BitInt(63))-1), 0);
CHECK_VALUE(((_BitInt(63))(-4611686018427387903LL - 1LL) / (_BitInt(63))1), (_BitInt(63))(-4611686018427387903LL - 1LL));
CHECK_VALUE(((_BitInt(63))(-4611686018427387903LL) / (_BitInt(63))-1), (_BitInt(63))4611686018427387903LL);
#endif
#endif

#ifdef TEST12
#ifdef __SDCC
/* _BitInt(64): overflow result, sign, remainder and representable controls. */
CHECK_VALUE(((_BitInt(64))(-9223372036854775807LL - 1LL) / (_BitInt(64))-1), (_BitInt(64))(-9223372036854775807LL - 1LL));
_Static_assert(((_BitInt(64))(-9223372036854775807LL - 1LL) / (_BitInt(64))-1) < 0, "negative quotient");
CHECK_VALUE(((_BitInt(64))(-9223372036854775807LL - 1LL) % (_BitInt(64))-1), 0);
CHECK_VALUE(((_BitInt(64))(-9223372036854775807LL - 1LL) / (_BitInt(64))1), (_BitInt(64))(-9223372036854775807LL - 1LL));
CHECK_VALUE(((_BitInt(64))(-9223372036854775807LL) / (_BitInt(64))-1), (_BitInt(64))9223372036854775807LL);
#endif
#endif
