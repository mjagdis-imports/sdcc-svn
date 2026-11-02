/* Fixed underlying enumeration types must agree across declarations. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

void unrelated (void);

#ifndef SKIP_C23
#ifdef TEST1
enum E : unsigned char;
enum E : unsigned int; /* ERROR(SDCC) */
#endif

#ifdef TEST2
enum E : unsigned char;
enum E : unsigned int { value = 1 }; /* ERROR(SDCC) */
#endif

#ifdef TEST3
enum E : unsigned char;
enum E : signed char; /* ERROR(SDCC) */
#endif

#ifdef TEST4
/* A definition of a fixed-underlying enumeration must retain the specifier. */
enum E : unsigned char;
enum E { value = 1 }; /* ERROR(SDCC) */
#endif

#ifdef TEST5
/* An enumeration without a fixed underlying type cannot acquire one. */
enum E { value = 1 };
enum E : unsigned char; /* ERROR(SDCC) */
#endif

#ifdef TEST6
enum E : unsigned char { value = 1 };
enum E : unsigned int; /* ERROR(SDCC) */
#endif

#ifdef TEST7
typedef unsigned char Byte;
enum E : Byte;
enum E : unsigned char;
enum E : Byte { value = 1 };
#endif

#ifdef TEST8
enum E : unsigned char;
enum E : unsigned char { value = 255 };
enum E object = value;
#endif

#ifdef TEST9
/* A new inner-scope tag may use another underlying type. */
enum E : unsigned char;
int test (void)
{
  enum E : unsigned int;
  enum E : unsigned int { value = 257 };
  return value;
}
#endif

#ifdef TEST10
/* Referring to a fixed type without defining it does not change fixedness. */
enum E : unsigned char;
enum E object;
enum E : unsigned char { value = 1 };
#endif

#ifdef TEST11
enum E { first = 1 };
enum E object = first;
#endif

#ifdef TEST12
/* Both declaration orders must check the established fixed type. */
enum E : unsigned char { value = 1 };
enum E : signed char { other = 1 }; /* ERROR(SDCC) */
#endif

#ifdef TEST13
/* Sibling blocks have equal levels but introduce distinct fixed enum tags. */
void test (void)
{
  {
    enum E : unsigned char;
  }
  {
    enum E : unsigned int;
    enum E : unsigned int { value = 257 };
    _Static_assert (sizeof (enum E) == sizeof (unsigned int), "second block type");
  }
}
#endif

#ifdef TEST14
/* Exercise the definition with an explicit underlying type in both blocks. */
void test (void)
{
  {
    enum E : unsigned char { value = 1 };
  }
  {
    enum E : unsigned int { value = 257 };
    _Static_assert (sizeof (enum E) == sizeof (unsigned int), "second block type");
  }
}
#endif

#ifdef TEST15
/* A definition without an underlying type belongs to the new block too. */
void test (void)
{
  {
    enum E : unsigned int { value = 1 };
  }
  {
    enum E { value = 1 };
    _Static_assert (value == 1, "second block enumerator");
  }
}
#endif

#ifdef TEST16
/* The new fixed tag must be recorded: later declarations must match it. */
void test (void)
{
  {
    enum E : unsigned int;
  }
  {
    enum E : unsigned int;
    enum E : signed int; /* ERROR(SDCC) */
  }
}
#endif

#ifdef TEST17
/* A typed definition in a sibling block must establish its own tag. */
void test (void)
{
  {
    enum E : unsigned int { value = 1 };
  }
  {
    enum E : unsigned int { value = 257 };
    enum E : signed int; /* ERROR(SDCC) */
  }
}
#endif

#ifdef TEST18
/* A new unfixed tag must not inherit fixedness from a sibling block. */
void test (void)
{
  {
    enum E : unsigned int { value = 1 };
  }
  {
    enum E { value = 1 };
    enum E : unsigned int; /* ERROR(SDCC) */
  }
}
#endif
#endif
