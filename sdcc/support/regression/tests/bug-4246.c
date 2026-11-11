/* Reusing an incomplete-array typedef preserves independent object bounds. */
#include <testfwk.h>

typedef int Tail[];
static Tail first = {11, 22};
static Tail second = {33, 44, 55};
struct S { int count; Tail data; };

void
test_incomplete_array_typedef (void)
{
  ASSERT (sizeof (first) == 2 * sizeof (int));
  ASSERT (sizeof (second) == 3 * sizeof (int));
  ASSERT (first[0] == 11);
  ASSERT (first[1] == 22);
  ASSERT (second[0] == 33);
  ASSERT (second[1] == 44);
  ASSERT (second[2] == 55);
  ASSERT (sizeof (struct S) >= sizeof (int));
}
