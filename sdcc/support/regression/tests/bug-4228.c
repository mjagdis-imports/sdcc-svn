/* Retain initialisers inspected for constant expressions (bug #4228). */
#include <testfwk.h>

#if __STDC_VERSION__ >= 202311L
constexpr int file_integer = _Generic (1, int: 7, default: 1.0);
constexpr float file_float = _Generic (1, int: 1.5f, default: 0);
constexpr int file_default = _Generic (1, float: 1.0f, default: 9);
constexpr int file_nested = _Generic (1, int: _Generic (1L, long: 11, default: 1.0), default: 2.0);
#endif

void test_file_scope(void)
{
#if __STDC_VERSION__ >= 202311L
  ASSERT(file_integer == 7);
  ASSERT(file_float == 1.5f);
  ASSERT(file_default == 9);
  ASSERT(file_nested == 11);
#endif
}

void test_block_scope(void)
{
#if __STDC_VERSION__ >= 202311L
  constexpr int integer = _Generic (1, int: 13, default: 1.0);
  constexpr float floating = _Generic (1, int: 2.5f, default: 0);
  constexpr int fallback = _Generic (1, float: 1.0f, default: 15);
  constexpr int nested = _Generic (1, int: _Generic (1L, long: 17, default: 1.0), default: 2.0);
  constexpr int sum = 1 + _Generic (1, int: 18, default: 1.0);
  constexpr int cast = (int)_Generic (1, int: 21, default: 1.0);
  constexpr int condition = 1 ? _Generic (1, int: 23, default: 1.0) : 0;
  constexpr int previous = _Generic (1, int: integer, default: 1.0);

  ASSERT(integer == 13);
  ASSERT(floating == 2.5f);
  ASSERT(fallback == 15);
  ASSERT(nested == 17);
  ASSERT(sum == 19);
  ASSERT(cast == 21);
  ASSERT(condition == 23);
  ASSERT(previous == 13);
#endif
}

void test_compound_literal(void)
{
#if __STDC_VERSION__ >= 201112L
  ASSERT((int){_Generic (1, int: 27, default: 1.0)} == 27);
  ASSERT((int){_Generic (1, float: 1.0f, default: 29)} == 29);
  ASSERT((int){_Generic (1, int: _Generic (1L, long: 31, default: 1.0), default: 2.0)} == 31);
#endif
}

void test_representable_initialisers(void)
{
#if __STDC_VERSION__ >= 202311L
  constexpr unsigned char maximum = _Generic (1, int: 255, default: 256);
  constexpr unsigned char fallback = _Generic (1, float: 256, default: 0);
  constexpr signed char nested = _Generic (1, int: _Generic (1L, long: 127, default: 128), default: 128);
  constexpr float exact = _Generic (1, int: 16777216L, default: 16777217L);
  constexpr float default_exact = _Generic (1, float: 16777217L, default: 16777216L);

  ASSERT(maximum == 255);
  ASSERT(fallback == 0);
  ASSERT(nested == 127);
  ASSERT(exact == 16777216.0f);
  ASSERT(default_exact == 16777216.0f);
#endif
}
