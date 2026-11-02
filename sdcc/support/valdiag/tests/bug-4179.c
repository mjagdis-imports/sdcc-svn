/* Numeric escapes must fit the unsigned character type of the literal. */
#include <stdint.h>
#include <stddef.h>

#ifdef __SDCC
#pragma std_c2y
#define HAS_C11_LITERALS
#define HAS_TYPEOF_UNQUAL
#define HAS_C23_CHARACTER_LITERALS
#define HAS_DELIMITED_ESCAPES
#elif defined(__STDC_VERSION__)

#if __STDC_VERSION__ >= 202311L
#define HAS_TYPEOF_UNQUAL
#define HAS_C23_CHARACTER_LITERALS
#endif
#if __STDC_VERSION__ > 202311L
#define HAS_DELIMITED_ESCAPES
#endif
#if __STDC_VERSION__ >= 201112L
#define HAS_C11_LITERALS
#endif
#endif

/* Optional syntax may be diagnosed or skipped in a host language mode. */
#ifdef HAS_TYPEOF_UNQUAL
#define CHAR16_TYPE typeof_unqual(*u"")
#define CHAR32_TYPE typeof_unqual(*U"")
#else
#define CHAR16_TYPE uint_least16_t
#define CHAR32_TYPE uint_least32_t
#endif

void unrelated (void);

#ifdef TEST1
int object = '\x100'; /* WARNING */
#endif

#ifdef TEST2
int object = '\400'; /* WARNING */
#endif

#ifdef TEST3
char object[] = "\x100"; /* WARNING */
#endif

#ifdef TEST4
char object[] = "\400"; /* WARNING */
#endif

#ifdef TEST5
/* Detect host strtoul overflow before any narrowing conversion. */
int object = '\x100000000000000000000000000000000'; /* WARNING */
#endif

#ifdef TEST6
char object[] = "\x100000000000000000000000000000000"; /* WARNING */
#endif

#ifdef TEST7
int object = '\xFF';
#endif

#ifdef TEST8
char object[] = "\177";
#endif

#ifdef TEST9
/* A backslash followed by ordinary x digits is not a numeric escape. */
char object[] = "\\x100";
#endif

#ifdef TEST10
#ifdef HAS_C23_CHARACTER_LITERALS
unsigned char object = u8'\xFF';
#endif
#endif

#ifdef TEST11
#ifdef HAS_C23_CHARACTER_LITERALS
unsigned char object = u8'\x100'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST12
#ifdef HAS_C11_LITERALS
unsigned int object = u'\x10000'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST13
#ifdef HAS_C11_LITERALS
unsigned long object = U'\x100000000'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST14
#ifdef HAS_DELIMITED_ESCAPES
char object[] = "\x{100}"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST15
#ifdef HAS_DELIMITED_ESCAPES
int object = '\o{400}'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST16
#ifdef HAS_C11_LITERALS
CHAR16_TYPE object[] = u"\x10000"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST17
#ifdef HAS_C11_LITERALS
CHAR32_TYPE object[] = U"\x100000000"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif
#ifdef TEST18
#ifdef HAS_C23_CHARACTER_LITERALS
/* A simple escape does not permit a second character. */
unsigned char object = u8'\nA'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST19
#ifdef HAS_C23_CHARACTER_LITERALS
/* A three-digit octal escape must end at the closing quote. */
unsigned char object = u8'\101B'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST20
#ifdef HAS_C23_CHARACTER_LITERALS
/* A non-hexadecimal character after the escape is still another character. */
unsigned char object = u8'\x41G'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST21
#ifdef HAS_DELIMITED_ESCAPES
/* Check the end of a delimited hexadecimal escape. */
unsigned char object = u8'\x{41}G'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST22
#ifdef HAS_DELIMITED_ESCAPES
/* Check the end of a delimited octal escape. */
unsigned char object = u8'\o{101}B'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST23
#ifdef HAS_C23_CHARACTER_LITERALS
/* An escaped backslash does not permit another character. */
unsigned char object = u8'\\A'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST24
#ifdef HAS_C23_CHARACTER_LITERALS
/* An escaped quote does not terminate the literal. */
unsigned char object = u8'\'B'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST25
#ifdef HAS_C23_CHARACTER_LITERALS
/* A valid universal character name must also end at the closing quote. */
unsigned char object = u8'\u0024B'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST26
#ifdef HAS_C23_CHARACTER_LITERALS
/* Two ordinary characters are invalid in a u8 character literal. */
unsigned char object = u8'AB'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST27
#ifdef HAS_C23_CHARACTER_LITERALS
/* A universal character name requiring two UTF-8 code units remains invalid. */
unsigned char object = u8'\u00A0'; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST28
#ifdef HAS_C23_CHARACTER_LITERALS
/* A source character requiring two UTF-8 code units remains invalid. */
unsigned char object = u8' '; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST29
#ifdef HAS_C23_CHARACTER_LITERALS
/* A single-code-unit universal character name is valid. */
unsigned char object = u8'\u0024';
#endif
#endif

#ifdef TEST30
#ifdef HAS_C23_CHARACTER_LITERALS
/* Numeric escapes can name a byte that is not an ASCII code point. */
unsigned char object = u8'\x80';
#endif
#endif

#ifdef TEST31
#ifdef HAS_C23_CHARACTER_LITERALS
/* The maximum octal byte value is valid. */
unsigned char object = u8'\377';
#endif
#endif

#ifdef TEST32
#ifdef HAS_DELIMITED_ESCAPES
/* The maximum delimited hexadecimal byte value is valid. */
unsigned char object = u8'\x{ff}';
#endif
#endif

#ifdef TEST33
#ifdef HAS_DELIMITED_ESCAPES
/* The maximum delimited octal byte value is valid. */
unsigned char object = u8'\o{377}';
#endif
#endif

#ifdef TEST34
#ifdef HAS_C11_LITERALS
/* The UTF-16 numeric escape limit is inclusive. */
unsigned int object = u'\xFFFF';
#endif
#endif

#ifdef TEST35
#ifdef HAS_C11_LITERALS
/* The UTF-32 numeric escape limit is inclusive. */
unsigned long object = U'\xFFFFFFFF';
#endif
#endif

#ifdef TEST36
#if WCHAR_MAX >= 0x7fffffffL
/* The wide-character numeric escape limit is inclusive. */
unsigned long object = L'\xFFFFFFFF';
#endif
#endif

#ifdef TEST37
#ifdef HAS_C11_LITERALS
/* A UTF-8 string numeric escape must fit one byte. */
unsigned char object[] = u8"\x100"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST38
#if WCHAR_MAX >= 0x7fffffffL && WCHAR_MAX <= 0xffffffffUL
/* A wide string numeric escape must fit its unsigned element type. */
wchar_t object[] = L"\x100000000"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST39
#ifdef HAS_DELIMITED_ESCAPES
/* Delimited octal conversion must detect host overflow. */
char object[] = "\o{777777777777777777777777777777777777777777}"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST40
#ifdef HAS_DELIMITED_ESCAPES
/* Delimited hexadecimal conversion must detect host overflow. */
char object[] = "\x{100000000000000000000000000000000}"; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST41
#ifdef HAS_DELIMITED_ESCAPES
/* Check the UTF-16 limit for delimited octal escapes. */
unsigned int object = u'\o{200000}'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST42
#ifdef HAS_DELIMITED_ESCAPES
/* Check the UTF-32 limit for delimited octal escapes. */
unsigned long object = U'\o{40000000000}'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST43
#ifdef HAS_DELIMITED_ESCAPES
/* Check the UTF-8 limit for delimited octal escapes. */
unsigned char object = u8'\o{400}'; /* WARNING(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST44
#ifdef HAS_DELIMITED_ESCAPES
/* An empty hexadecimal escape is malformed. */
char object[] = "\x{}"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST45
#ifdef HAS_DELIMITED_ESCAPES
/* An empty octal escape is malformed. */
char object[] = "\o{}"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST46
#ifdef HAS_DELIMITED_ESCAPES
/* A delimited hexadecimal escape needs a closing brace. */
char object[] = "\x{1"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST47
#ifdef HAS_DELIMITED_ESCAPES
/* A delimited octal escape needs a closing brace. */
char object[] = "\o{1"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST48
/* Escaped backslashes do not begin numeric or universal escapes. */
char object[] = "\\x100\\377\\u0041";
#endif

#ifdef TEST49
/* Concatenation preserves the distinction between escaped backslashes and numeric escapes. */
unsigned char object[] = "\\x100" "\x7F";
#endif

#ifdef TEST50
/* A numeric string escape within the signed and unsigned character ranges is valid. */
unsigned char object[] = "\x7F";
#endif
