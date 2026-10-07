/* Function attributes must not separate return pointers from their type. */
#include <testfwk.h>

typedef char **PointerReturnFunction(void) __reentrant;
typedef char ***TriplePointerReturnFunction(void) __reentrant;

static char character;
static char *pointer = &character;
static char **pointer_to_pointer = &pointer;

static char **getPointer(void) __reentrant
{
  return &pointer;
}

static char ***getPointerToPointer(void) __reentrant
{
  return &pointer_to_pointer;
}

void testBug(void)
{
  PointerReturnFunction *pointer_return_function = getPointer;
  TriplePointerReturnFunction *triple_pointer_return_function =
    getPointerToPointer;

  ASSERT(pointer_return_function() == &pointer);
  ASSERT(*pointer_return_function() == &character);
  ASSERT(triple_pointer_return_function() == &pointer_to_pointer);
  pointer_return_function = 0;
  triple_pointer_return_function = 0;
  ASSERT(!pointer_return_function);
  ASSERT(!triple_pointer_return_function);
}
