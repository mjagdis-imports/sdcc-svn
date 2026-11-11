/* Valid uses of unions containing flexible-array structures. */
#include <testfwk.h>

struct Flexible { int count; char data[]; };
union Payload { struct Flexible flexible; int number; };
union Nested { union Payload payload; long number; };

static union Payload object = {.number = 27};
static union Nested nested = {.number = 1234};
static union Payload *pointers[] = {&object};
struct Container { union Payload *pointer; };
static struct Container container = {&object};

void
test_flexible_union (void)
{
  ASSERT (object.number == 27);
  ASSERT (nested.number == 1234);
  ASSERT (pointers[0] == &object);
  ASSERT (container.pointer->number == 27);
  ASSERT (sizeof (union Payload) >= sizeof (struct Flexible));
  ASSERT (sizeof (union Nested) >= sizeof (union Payload));
}
