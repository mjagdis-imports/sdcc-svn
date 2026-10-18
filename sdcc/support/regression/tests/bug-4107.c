/* Equivalent function-pointer spellings must select the expected association. */
#include <testfwk.h>

typedef int function_type (int);
typedef function_type *pointer_type;
int f (int);
pointer_type pf;

#ifdef __SDCC_mcs51
typedef int banked_function_type (int) __banked;
typedef banked_function_type *banked_pointer_type;
int banked_f (int) __banked;
banked_pointer_type banked_pf;

__xdata int *xp;
__data int *dp;
#endif

void
testFunctionPointerTypes (void)
{
  /* Equivalent association spellings must match the declared pointer.
     The baseline selects default for some equivalent spellings. */
  ASSERT (_Generic (pf, function_type *: 1, default: 0));
  ASSERT (_Generic (pf, pointer_type: 1, default: 0));
  ASSERT (_Generic (pf, int (*)(int): 1, default: 0));

  /* Taking the function address must produce the same pointer type. */
  ASSERT (_Generic (&f, function_type *: 1, default: 0));
  ASSERT (_Generic (&f, pointer_type: 1, default: 0));
  ASSERT (_Generic (&f, int (*)(int): 1, default: 0));

  /* Function-designator conversion must agree with the declared pointer. */
  ASSERT (_Generic (f, function_type *: 1, default: 0));
  ASSERT (_Generic (f, pointer_type: 1, default: 0));
  ASSERT (_Generic (f, int (*)(int): 1, default: 0));

  /* Dereferencing the pointer yields a function designator that converts
     back to the same pointer type. A mismatch selects default. */
  ASSERT (_Generic (*pf, function_type *: 1, default: 0));
  ASSERT (_Generic (*pf, pointer_type: 1, default: 0));
  ASSERT (_Generic (*pf, int (*)(int): 1, default: 0));

  /* An extra pointer level and sizeof must preserve typedef equivalence. */
  ASSERT (_Generic (&pf, function_type **: 1, default: 0));
  ASSERT (_Generic (&pf, pointer_type *: 1, default: 0));

  /* An object-pointer association must remain distinct from a function pointer. */
  ASSERT (_Generic (pf, int *: 0, function_type *: 1, default: 0));

#ifdef __SDCC_mcs51
  /* Banked function pointers must also agree across equivalent spellings. */
  ASSERT (_Generic (banked_pf, banked_function_type *: 1, default: 0));
  ASSERT (_Generic (banked_pf, banked_pointer_type: 1, default: 0));
  ASSERT (_Generic (&banked_f, banked_function_type *: 1, default: 0));
  ASSERT (_Generic (&banked_f, banked_pointer_type: 1, default: 0));
  ASSERT (_Generic (banked_f, banked_function_type *: 1, default: 0));
  ASSERT (_Generic (banked_f, banked_pointer_type: 1, default: 0));
  /* Banked and ordinary function pointers must not become interchangeable. */
  ASSERT (_Generic (pf, banked_pointer_type: 0, pointer_type: 1));
  ASSERT (_Generic (banked_pf, pointer_type: 0, banked_pointer_type: 1));
  ASSERT (sizeof (banked_function_type *) == sizeof (banked_pointer_type));
#endif

#ifdef __SDCC_mcs51
  /* Preservation: object address spaces remain distinct. */
  ASSERT (_Generic (xp, __xdata int *: 1, __data int *: 0));
  ASSERT (_Generic (dp, __data int *: 1, __xdata int *: 0));
#endif
}
