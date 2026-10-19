/* Valid pointer combinations must keep their type and selected address. */
#include <testfwk.h>

void testCompatibleConditionalPointers (void)
{
  int values[2] = { 3, 5 };
  volatile int condition = 1;
  int *p = values;
  const int *q = values + 1;
  volatile int *r = values;
  void *v = values + 1;

  /* Check expression types before assignment can add qualifications. */
  ASSERT (_Generic (condition ? p : q, const int *: 1, default: 0));
  ASSERT (_Generic (condition ? q : r, const volatile int *: 1, default: 0));
  ASSERT (_Generic (condition ? p : v, void *: 1, default: 0));
  ASSERT (_Generic (condition ? v : p, void *: 1, default: 0));
  ASSERT (_Generic (condition ? 0 : p, int *: 1, default: 0));
  ASSERT (_Generic (condition ? p : 0, int *: 1, default: 0));

  /* Each combination must select the first operand when the condition is true. */
  ASSERT ((condition ? p : q) == values);
  ASSERT ((condition ? q : r) == values + 1);
  ASSERT ((condition ? p : v) == values);
  ASSERT ((condition ? v : p) == values + 1);
  ASSERT ((condition ? 0 : p) == 0);
  ASSERT ((condition ? p : 0) == values);
  ASSERT ((condition ? values : q) == values);

  /* The false branch is checked against independently named array addresses. */
  condition = 0;
  ASSERT ((condition ? p : q) == values + 1);
  ASSERT ((condition ? q : r) == values);
  ASSERT ((condition ? p : v) == values + 1);
  ASSERT ((condition ? v : p) == values);
  ASSERT ((condition ? 0 : p) == values);
  ASSERT ((condition ? p : 0) == 0);
  ASSERT ((condition ? values : q) == values + 1);
}
