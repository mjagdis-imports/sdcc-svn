/* A whole-object store must not reuse a bit-field access at the same address. */
#include <testfwk.h>

union U
{
  struct
  {
    unsigned char a : 4;
    unsigned char b : 4;
  } s;
  unsigned char raw;
};

/* CSE used the last initialised bit-field's pointer for the byte store. */
void test_initialised_union(void)
{
  union U v = {{1, 2}};

  v.raw = 0x55;
  ASSERT(v.raw == 0x55);
}

static unsigned char store_byte(unsigned char value)
{
  union U v = {{1, 2}};

  v.raw = value;
  return v.raw;
}

/* The replacement must also preserve a store whose value is not constant. */
void test_runtime_byte_store(void)
{
  ASSERT(store_byte(0) == 0);
  ASSERT(store_byte(0x55) == 0x55);
  ASSERT(store_byte(0xa3) == 0xa3);
  ASSERT(store_byte(0xff) == 0xff);
}

/* A low bit-field access must not restrict a later whole-byte store. */
void test_low_field_store(void)
{
  union U v;

  v.s.a = 1;
  v.raw = 0xa5;
  ASSERT(v.raw == 0xa5);
}

/* A bit-field write must still preserve the neighbouring field. */
void test_neighbouring_field(void)
{
  union U v;

  v.raw = 0x55;
  v.s.b = 10;
  ASSERT(v.s.a == 5);
  ASSERT(v.s.b == 10);
}

/* The same address substitution can occur for an explicitly volatile union. */
void test_volatile_union(void)
{
  volatile union U v = {{1, 2}};

  v.raw = 0x55;
  ASSERT(v.raw == 0x55);
}

/* Static storage must not hide the effect of an earlier bit-field store. */
void test_static_union(void)
{
  static union U v;

  v.s.b = 2;
  v.raw = 0x55;
  ASSERT(v.raw == 0x55);
}

/* A nonzero enclosing member offset must not change the access type. */
void test_nested_union(void)
{
  struct S
  {
    unsigned char before;
    union U member;
    unsigned char after;
  } v = {7, {{1, 2}}, 9};

  v.member.raw = 0x55;
  ASSERT(v.before == 7);
  ASSERT(v.member.raw == 0x55);
  ASSERT(v.after == 9);
}

/* A different field width must not mask or shift a whole-byte value. */
void test_unequal_fields(void)
{
  union
  {
    struct
    {
      unsigned char a : 3;
      unsigned char b : 5;
    } s;
    unsigned char raw;
  } v = {{1, 2}};

  v.raw = 0xa5;
  ASSERT(v.raw == 0xa5);
}

/* A whole-word store must not inherit the size of a bit-field access. */
void test_word_store(void)
{
  union
  {
    struct
    {
      unsigned int a : 4;
      unsigned int b : 4;
    } s;
    unsigned int raw;
  } v = {{1, 2}};

  v.raw = 0xa55a;
  ASSERT(v.raw == 0xa55a);
}
