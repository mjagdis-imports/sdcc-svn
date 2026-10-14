/* Explicitly typed auto objects retain their declared types and conversions. */

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23_TESTS
#endif

static int calls;

static int
nextValue(void)
{
  ++calls;
  return 7;
}

void
testExplicitAutoTypes(void)
{
  /* Ordinary initialisation converts to the explicitly declared type. */
  auto int integer = 2.5f;
  _Static_assert(_Generic(&integer, int *: 1, default: 0), "int object");
  auto float floating = 2;
  _Static_assert(_Generic(&floating, float *: 1, default: 0), "float object");
  auto unsigned char small = 200;
  _Static_assert(_Generic(&small, unsigned char *: 1, default: 0), "unsigned char object");
  ASSERT(integer == 2);
  ASSERT(floating == 2.0f);
  ASSERT(small == 200);

  /* Length and signedness specifiers imply int when no noun is written. */
  auto unsigned positive = 2.5f;
  _Static_assert(_Generic(&positive, unsigned int *: 1, default: 0), "unsigned int object");
  auto signed signed_value = -2.5f;
  _Static_assert(_Generic(&signed_value, signed int *: 1, default: 0), "signed int object");
  auto short short_value = 2.5f;
  _Static_assert(_Generic(&short_value, short int *: 1, default: 0), "short int object");
  auto long long_value = 2.5f;
  _Static_assert(_Generic(&long_value, long int *: 1, default: 0), "long int object");
  auto long long wide = 2.5f;
  _Static_assert(_Generic(&wide, long long int *: 1, default: 0), "long long int object");
  ASSERT(positive == 2);
  ASSERT(signed_value == -2);
  ASSERT(short_value == 2);
  ASSERT(long_value == 2);
  ASSERT(wide == 2);

  /* A typedef supplies the type, and its initialiser is evaluated once. */
  typedef int I;
  calls = 0;
  auto I alias = nextValue();
  _Static_assert(_Generic(&alias, I *: 1, default: 0), "I object");
  ASSERT(alias == 7);
  ASSERT(calls == 1);

  /* Aggregate, pointer and braced initialisation retain their behaviour. */
  struct S
  {
    int member;
  };
  struct S source = {3};
  auto struct S copy = source;
  _Static_assert(_Generic(&copy, struct S *: 1, default: 0), "struct S object");
  auto int *pointer = &integer;
  _Static_assert(_Generic(&pointer, int **: 1, default: 0), "int * object");
  auto int braced = {4};
  _Static_assert(_Generic(&braced, int *: 1, default: 0), "int object");
  auto int assigned;
  _Static_assert(_Generic(&assigned, int *: 1, default: 0), "int object");
  assigned = 5;
  ASSERT(copy.member == 3);
  ASSERT(*pointer == 2);
  ASSERT(braced == 4);
  ASSERT(assigned == 5);

#ifndef SKIP_C23_TESTS
  /* Omitting a type still permits inference, including explicit qualifiers. */
  auto inferred = 6;
  _Static_assert(_Generic(&inferred, int *: 1, default: 0), "int object");
  auto const qualified = 8;
  _Static_assert(_Generic(&qualified, const int *: 1, default: 0), "const int object");
  ASSERT(inferred == 6);
  ASSERT(qualified == 8);

  /* Inference retains unsigned char without promotion and retains float. */
  auto inferred_small = small;
  auto inferred_float = 2.5f;
  _Static_assert(_Generic(&inferred_small, unsigned char *: 1, default: 0), "unsigned char object");
  _Static_assert(_Generic(&inferred_float, float *: 1, default: 0), "float object");
  ASSERT(inferred_small == 200);
  ASSERT(inferred_float == 2.5f);
#endif
}
