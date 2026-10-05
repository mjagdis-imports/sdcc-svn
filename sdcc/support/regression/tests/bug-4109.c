/* Semantic dereferences preserve pointer values and single evaluation. */

#include <testfwk.h>

#if defined(__SDCC) // TODO: When _Optional becmoes part of standard C (or there is a preprocessor macro to indictae support in the C stdnard or _Optional TS), conditionally enable test for host compiler.
static int evaluations;
static int object;

static _Optional int *get_pointer(void)
{
  ++evaluations;
  return &object;
}

#pragma disable_warning 355
static int *evaluated_pointer(void)
{
  int *q = &*get_pointer();
  return q;
}

static int *copy_pointer(_Optional int *p)
{
  if (p)
    {
      int *q = &*p;
      return q;
    }
  return 0;
}
#endif

void testSemanticDereference(void)
{
#if defined(__SDCC)
  evaluations = 0;
  ASSERT(evaluated_pointer() == &object);
  ASSERT(evaluations == 1);
  ASSERT(copy_pointer(get_pointer()) == &object);
  ASSERT(evaluations == 2);
  ASSERT(copy_pointer(0) == 0);
  ASSERT(evaluations == 2);
#endif
}

