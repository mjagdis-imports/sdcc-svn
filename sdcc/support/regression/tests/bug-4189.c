/* Matching wide character types retain their initialiser data. */
#include <testfwk.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __SDCC
#pragma std_c23
#define HAS_C11_LITERALS
#define HAS_TYPEOF_UNQUAL
#elif defined(__STDC_VERSION__)

#if __STDC_VERSION__ >= 202311L
#define HAS_TYPEOF_UNQUAL
#endif
#if __STDC_VERSION__ >= 201112L
#define HAS_C11_LITERALS
#endif
#endif

#ifdef HAS_TYPEOF_UNQUAL
#define CHAR16_TYPE typeof_unqual(*u"")
#define CHAR32_TYPE typeof_unqual(*U"")
/* Select wchar_t only where it is compatible with the string element type. */
#define CHAR32_COMPATIBLE_TYPE typeof_unqual(_Generic((wchar_t)0, uint_least32_t: (wchar_t)0, default: (uint_least32_t)0))
#define WCHAR_COMPATIBLE_TYPE typeof_unqual(_Generic((uint_least32_t)0, wchar_t: (uint_least32_t)0, default: (wchar_t)0))
#else
#define CHAR16_TYPE uint_least16_t
#define CHAR32_TYPE uint_least32_t
#endif

#ifdef HAS_C11_LITERALS
static const CHAR16_TYPE utf16[4] = u"xy";
static const CHAR32_TYPE utf32[4] = {U"pq"};
#endif
static const wchar_t wide[4] = L"rs";
#ifdef HAS_TYPEOF_UNQUAL
static const CHAR32_COMPATIBLE_TYPE compatible[4] = U"rs";
#endif
static const char *pointers[1] = {"x"};
static const char rows[1][2] = {"y"};
#ifdef HAS_C11_LITERALS
static const CHAR16_TYPE wide_rows[1][2] = {u"z"};
#endif

void
testMatchingWideInitialisers (void)
{
#ifdef HAS_C11_LITERALS
  CHAR16_TYPE local[4] = {u"ab"};
  CHAR16_TYPE local_wide_rows[1][2] = {u"r"};
#endif
  const char *local_pointers[1] = {"p"};
  char local_rows[1][2] = {"q"};
#ifdef HAS_C11_LITERALS
  ASSERT (utf16[0] == 'x');
  ASSERT (utf16[1] == 'y');
  ASSERT (utf16[2] == 0);
  ASSERT (utf16[3] == 0);
  ASSERT (utf32[0] == 'p');
  ASSERT (utf32[1] == 'q');
  ASSERT (utf32[2] == 0);
#endif
  ASSERT (wide[0] == 'r');
  ASSERT (wide[1] == 's');
  ASSERT (wide[2] == 0);
#ifdef HAS_C11_LITERALS
  ASSERT (local[0] == 'a');
  ASSERT (local[1] == 'b');
  ASSERT (local[2] == 0);
  ASSERT (local[3] == 0);
#endif
#ifdef HAS_TYPEOF_UNQUAL
  ASSERT (compatible[0] == 'r');
  ASSERT (compatible[1] == 's');
  ASSERT (compatible[2] == 0);
#endif
  /* Strings must initialise the selected pointer or inner array. */
  ASSERT (pointers[0][0] == 'x');
  ASSERT (pointers[0][1] == 0);
  ASSERT (rows[0][0] == 'y');
  ASSERT (rows[0][1] == 0);
#ifdef HAS_C11_LITERALS
  ASSERT (wide_rows[0][0] == 'z');
  ASSERT (wide_rows[0][1] == 0);
#endif
  ASSERT (local_pointers[0][0] == 'p');
  ASSERT (local_pointers[0][1] == 0);
  ASSERT (local_rows[0][0] == 'q');
  ASSERT (local_rows[0][1] == 0);
#ifdef HAS_C11_LITERALS
  ASSERT (local_wide_rows[0][0] == 'r');
  ASSERT (local_wide_rows[0][1] == 0);
#endif
}
