/* Bug #4266. Omitted automatic array elements must be initialised to zero. */
#include <testfwk.h>

void test_leading (void)
{
  volatile int array[3] = {[2] = 7};
  ASSERT (array[0] == 0);
  ASSERT (array[1] == 0);
  ASSERT (array[2] == 7);
}

void test_backwards (void)
{
  volatile int array[3] = {[2] = 7, [0] = 3};
  ASSERT (array[0] == 3);
  ASSERT (array[1] == 0);
  ASSERT (array[2] == 7);
}

void test_unknown_bound (void)
{
  volatile int array[] = {[3] = 9};
  ASSERT (sizeof array == 4 * sizeof (int));
  ASSERT (array[0] == 0);
  ASSERT (array[1] == 0);
  ASSERT (array[2] == 0);
  ASSERT (array[3] == 9);
}

void test_aggregate_elements (void)
{
  struct Pair { int x, y; };
  volatile struct Pair array[3] = {[2] = {7, 8}};
  ASSERT (array[0].x == 0);
  ASSERT (array[0].y == 0);
  ASSERT (array[1].x == 0);
  ASSERT (array[1].y == 0);
  ASSERT (array[2].x == 7);
  ASSERT (array[2].y == 8);
}

void test_internal_gap (void)
{
  volatile int array[6] = {2, [3] = 7, 8};
  ASSERT (array[0] == 2);
  ASSERT (array[1] == 0);
  ASSERT (array[2] == 0);
  ASSERT (array[3] == 7);
  ASSERT (array[4] == 8);
  ASSERT (array[5] == 0);
}
