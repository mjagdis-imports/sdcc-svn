/* Preserve values of valid numeric escapes, including UTF-8 code units. */
#include <testfwk.h>
#include <stdint.h>

#ifdef __SDCC
#pragma std_c2y
#define HAS_C11_LITERALS
#define HAS_UTF8_CHARACTER_LITERALS
#define HAS_DELIMITED_ESCAPES
#elif defined(__STDC_VERSION__)

#if __STDC_VERSION__ >= 201112L
#define HAS_C11_LITERALS
#endif
#if __STDC_VERSION__ >= 202311L
#define HAS_UTF8_CHARACTER_LITERALS
#endif
#if __STDC_VERSION__ > 202311L
#define HAS_DELIMITED_ESCAPES
#endif
#endif

static unsigned char bytes[] = "\xFF\377\177\0";
static char backslash[] = "\\x100";
static unsigned char simple[] = "\a\b\t\n\v\f\r\\\'\"\?";

void
testNumericEscapes (void)
{
  ASSERT ((unsigned char)'\xFF' == 255);
  ASSERT ((unsigned char)'\377' == 255);
  ASSERT (bytes[0] == 255);
  ASSERT (bytes[1] == 255);
  ASSERT (bytes[2] == 127);
  ASSERT (bytes[3] == 0);
  ASSERT (backslash[0] == '\\');
  ASSERT (backslash[1] == 'x');
  ASSERT (backslash[4] == '0');
#ifdef HAS_UTF8_CHARACTER_LITERALS
  /* Numeric escapes specify code units, rather than Unicode code points. */
  ASSERT (u8'\xFF' == 255);
  ASSERT (u8'\377' == 255);
  ASSERT (u8'\n' == 10);
  ASSERT (u8'A' == 65);
#endif
  /* The shared parser must preserve all simple escapes and their order. */
  ASSERT (simple[0] == 7);
  ASSERT (simple[1] == 8);
  ASSERT (simple[2] == 9);
  ASSERT (simple[3] == 10);
  ASSERT (simple[4] == 11);
  ASSERT (simple[5] == 12);
  ASSERT (simple[6] == 13);
  ASSERT (simple[7] == 92);
  ASSERT (simple[8] == 39);
  ASSERT (simple[9] == 34);
  ASSERT (simple[10] == 63);
  ASSERT (simple[11] == 0);
#ifdef HAS_UTF8_CHARACTER_LITERALS
  /* Numeric byte escapes and single-code-unit UCNs use different rules. */
  ASSERT (u8'\x80' == 128);
  ASSERT (u8'\u0024' == 36);
  ASSERT (u8'\'' == 39);
  ASSERT (u8'\\' == 92);
#endif
#ifdef HAS_DELIMITED_ESCAPES
  /* Delimited numeric escapes share parsing and retain boundary values. */
  ASSERT (u8'\x{ff}' == 255);
  ASSERT (u8'\o{377}' == 255);
#endif
#ifdef HAS_C11_LITERALS
  ASSERT (u'\xFFFF' == 65535U);
  ASSERT (U'\xFFFFFFFF' == 4294967295UL);
#if WCHAR_MAX >= 0x7fffffffL
  ASSERT ((uint_least32_t)L'\xFFFFFFFF' == 4294967295UL);
#endif
#endif

}
