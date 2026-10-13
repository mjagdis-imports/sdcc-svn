/* Type inference uses value conversions and evaluates the initialiser once. */
#include <testfwk.h>
#ifdef __SDCC
#pragma std_c23
#endif

static int calls;

static int
next(int value)
#ifdef SDCC_PDK
__reentrant
#endif
{
  ++calls;
  return value + 1;
}

void
testInferenceConversions(void)
{
  const int source = 3;
  auto copy = source;
  ++copy;
  ASSERT(copy == 4);
  ASSERT(source == 3);

  volatile int input = 6;
  auto value = input;
  ASSERT(_Generic(&value, int *: 1, default: 0));
  ASSERT(value == 6);

  int array[2] = {4, 5};
  auto pointer = array;
  pointer[1] = 7;
  ASSERT(array[1] == 7);

  int *const original = array;
  auto cursor = original;
  ++cursor;
  ASSERT(*cursor == 7);

  int matrix[2][2] = {{1, 2}, {3, 4}};
  auto row = matrix;
  row[1][0] = 9;
  ASSERT(matrix[1][0] == 9);

  auto text = "abc";
  ++text;
  ASSERT(*text == 'b');

  calls = 0;
  auto function = next;
  ASSERT(function(4) == 5);
  ASSERT(calls == 1);

  auto result = next(5);
  ASSERT(result == 6);
  ASSERT(calls == 2);

  auto address = &next;
  ASSERT(address(6) == 7);
  ASSERT(calls == 3);

  struct S
  {
    int member;
  };
  const struct S aggregate = {1};
  auto aggregate_copy = aggregate;
  aggregate_copy.member = 2;
  ASSERT(aggregate_copy.member == 2);
  ASSERT(aggregate.member == 1);

  unsigned char small = 200;
  auto small_copy = small;
  ASSERT(sizeof(small_copy) == sizeof(small));
  ASSERT(small_copy == 200);
}
