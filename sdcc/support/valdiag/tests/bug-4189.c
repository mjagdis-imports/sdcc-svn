/* String initialisers require matching character element types. */
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

/* Optional syntax may be diagnosed or skipped in a host language mode. */
void unrelated (void);

#ifdef TEST1
#ifdef HAS_C11_LITERALS
CHAR16_TYPE object[8] = U"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST2
#ifdef HAS_C11_LITERALS
CHAR32_TYPE object[8] = u"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST3
#ifdef HAS_C11_LITERALS
CHAR16_TYPE object[8] = {U"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST4
#ifdef HAS_C11_LITERALS
CHAR16_TYPE object[8] = U""; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST5
#ifdef HAS_C11_LITERALS
CHAR32_TYPE object[8] = u""; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST6
#ifdef HAS_C11_LITERALS
void test (void)
{
  CHAR16_TYPE object[8] = U"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
  (void)object;
}
#endif
#endif

#ifdef TEST7
#ifdef HAS_C11_LITERALS
typedef CHAR16_TYPE Character;
Character object[8] = U"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST8
#ifdef HAS_C11_LITERALS
const CHAR16_TYPE object[8] = U"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST9
#ifdef HAS_C11_LITERALS
CHAR16_TYPE object[8] = u"x";
#endif
#endif

#ifdef TEST10
#ifdef HAS_C11_LITERALS
const CHAR32_TYPE object[8] = {U"x"};
#endif
#endif

#ifdef TEST11
#ifdef HAS_TYPEOF_UNQUAL
/* Use wchar_t for a UTF-32 string wherever those element types are compatible. */
CHAR32_COMPATIBLE_TYPE object[8] = U"x";
#endif
#endif

#ifdef TEST12
#ifdef HAS_TYPEOF_UNQUAL
/* Use char32_t for a wide string wherever those element types are compatible. */
WCHAR_COMPATIBLE_TYPE object[8] = L"x";
#endif
#endif

#ifdef TEST13
/* Character arrays may use either plain or unsigned character elements. */
unsigned char object[8] = "x";
#endif

#ifdef TEST14
#ifdef HAS_C11_LITERALS
int test (void)
{
  CHAR16_TYPE object[8] = {u"x"};
  return object[0];
}
#endif
#endif

#ifdef TEST15
/* A string initialises a pointer element, not the enclosing array. */
const char *object[1] = {"x"};
#endif

#ifdef TEST16
/* A string can initialise one row of a character array. */
char object[1][2] = {"x"};
#endif

#ifdef TEST17
#ifdef HAS_C11_LITERALS
/* Check the element type of the inner wide array. */
CHAR16_TYPE object[1][2] = {u"x"};
#endif
#endif

#ifdef TEST18
/* Local pointer arrays retain ordinary string initialisation. */
int test (void)
{
  const char *object[1] = {"x"};
  return object[0][0];
}
#endif

#ifdef TEST19
/* Local character rows also remain valid. */
int test (void)
{
  char object[1][2] = {"x"};
  return object[0][0];
}
#endif

#ifdef TEST20
#ifdef HAS_C11_LITERALS
/* Local wide character rows also remain valid. */
int test (void)
{
  CHAR16_TYPE object[1][2] = {u"x"};
  return object[0][0];
}
#endif
#endif

#ifdef TEST21
/* Signed character arrays accept ordinary strings. */
signed char object[8] = "x";
#endif

#ifdef TEST22
#ifdef HAS_C11_LITERALS
/* Immediate volatile qualification does not change the element type. */
volatile CHAR16_TYPE object[8] = u"x";
#endif
#endif

#ifdef TEST23
#ifdef HAS_C11_LITERALS
/* Unknown array length does not permit mismatched element types. */
CHAR32_TYPE object[] = {u"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST24
#ifdef HAS_C11_LITERALS
/* A mismatched string inside a nested array must still be diagnosed. */
CHAR16_TYPE object[1][2] = {U"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST25
/* Multiple pointer elements remain valid. */
const char *object[2] = {"a", "b"};
#endif

#ifdef TEST26
/* A string can initialise a character-array structure member. */
struct S { char text[2]; };
struct S object[1] = {{"x"}};
#endif

#ifdef TEST27
#ifdef HAS_C11_LITERALS
/* An L-prefixed string cannot initialise incompatible UTF-16 elements.
   File-scope initialisation. */
CHAR16_TYPE object[8] = L"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST28
#ifdef HAS_C11_LITERALS
/* A u-prefixed string cannot initialise incompatible wchar_t elements.
   File-scope initialisation. */
wchar_t object[8] = u"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST29
#ifdef HAS_C11_LITERALS
/* An L-prefixed string cannot initialise incompatible UTF-16 elements.
   File-scope braced initialisation. */
CHAR16_TYPE object[8] = {L"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST30
#ifdef HAS_C11_LITERALS
/* A u-prefixed string cannot initialise incompatible wchar_t elements.
   File-scope braced initialisation. */
wchar_t object[8] = {u"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST31
#ifdef HAS_C11_LITERALS
/* An L-prefixed string cannot initialise incompatible UTF-16 elements.
   Local initialisation. */
void test (void)
{
  CHAR16_TYPE object[8] = L"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
  (void)object;
}
#endif
#endif

#ifdef TEST32
#ifdef HAS_C11_LITERALS
/* A u-prefixed string cannot initialise incompatible wchar_t elements.
   Local initialisation. */
void test (void)
{
  wchar_t object[8] = u"x"; /* ERROR(SDCC) */ /* IGNORE(GCC) */
  (void)object;
}
#endif
#endif

#ifdef TEST33
#ifdef HAS_C11_LITERALS
/* An L-prefixed string cannot initialise incompatible UTF-16 elements.
   Local braced initialisation. */
void test (void)
{
  CHAR16_TYPE object[8] = {L"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
  (void)object;
}
#endif
#endif

#ifdef TEST34
#ifdef HAS_C11_LITERALS
/* A u-prefixed string cannot initialise incompatible wchar_t elements.
   Local braced initialisation. */
void test (void)
{
  wchar_t object[8] = {u"x"}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
  (void)object;
}
#endif
#endif

#ifdef TEST35
/* A braced matching wide string is valid at file scope. */
wchar_t object[8] = {L"x"};
#endif

#ifdef TEST36
/* A braced matching wide string is also valid in a block. */
void test (void)
{
  wchar_t object[8] = {L"x"};
  (void)object;
}
#endif
