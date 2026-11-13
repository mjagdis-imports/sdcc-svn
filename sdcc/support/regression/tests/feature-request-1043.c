/* Const objects whose addresses escape (RFE #1043). */
#include <testfwk.h>

static int observed;
static int *observed_pointer;
static int replacement;

const char global_value = 9;
int global_pointee;
int *const global_pointer = &global_pointee;

static const char static_global_value = 11;
static int *const static_global_pointer = &global_pointee;

char mutable_global_value;
int *mutable_global_pointer;

static void
observe_char(const char *p) __reentrant
{
  /* An incorrect load would fail the caller's observed-value check. */
  observed = *p;
}

static char
constant_after_call(void)
{
  /* Returning anything other than 9 after the call fails the caller's check. */
  const char value = 9;
  observe_char(&value);
  return value;
}

static char
runtime_constant_after_call(char input)
{
  /* The caller checks that the returned and observed values equal input. */
  /* The initialiser need not be an integer constant expression. */
  const char value = input;
  observe_char(&value);
  return value;
}

static void
observe_pointer(int *const *p)
{
  /* An incorrect pointer load would fail the caller's observed-address
     check. */
  observed_pointer = *p;
}

static void
modify_pointee(int *const *p)
{
  /* Dropping this write would leave 10 instead of 23 in the caller. */
  **p = 23; /* The pointer is const; its pointee is mutable. */
}

static void
replace_pointer(int *const *p)
{
  /* Reusing the old pointer after this write would fail the caller's address
     check. */
  /* The caller supplies a mutable pointer object, so this is valid. */
  *(int **)p = &replacement;
}

static void
replace_char(const char *p)
{
  /* Reusing the old character after this write would fail the caller's value
     check. */
  /* The caller supplies a mutable character object. */
  *(char *)p = 17;
}

void
test_const_scalar(void)
{
  /* Both calls must observe and return their initial values: 9 and 31.
     These runtime checks catch wrong values; valdiag checks optimisation. */
  ASSERT(constant_after_call() == 9);
  ASSERT(observed == 9);
  ASSERT(runtime_constant_after_call(31) == 31);
  ASSERT(observed == 31);
}

void
test_const_pointer(void)
{
  /* The calls must leave saved pointing to value, but change value to 23.
     Treating the pointee as immutable would reuse 10 and fail the last check. */
  int value = 10;
  int *const saved = &value;
  observe_pointer(&saved);
  modify_pointee(&saved);
  ASSERT(saved == &value);
  ASSERT(observed_pointer == &value);
  ASSERT(*saved == 23);
}

void
test_mutable_objects(void)
{
  /* Reusing pre-call values would fail the 17 or replacement-address checks. */
  char value = 9;
  replace_char(&value);
  ASSERT(value == 17);

  int other = 10;
  int *saved = &other;
  replace_pointer(&saved);
  ASSERT(saved == &replacement);
}

void
test_indirect_call(void)
{
  /* The indirect call must read 9 through &value and store it in observed. */
  const char value = 9;
  void (*volatile call)(const char *) __reentrant = observe_char;
  call(&value);
  ASSERT(value == 9);
  ASSERT(observed == 9);
}

void
test_repeated_initialisation(void)
{
  /* Reusing a previous iteration's value would fail the comparisons with i. */
  for (int i = 0; i < 3; i++)
    {
      const char value = i;
      observe_char(&value);
      ASSERT(value == i);
      ASSERT(observed == i);
    }
}

void
test_const_globals(void)
{
  /* global_value stays 9 and global_pointer still points to global_pointee.
     Treating the pointee as const would reuse 10 after the write, not 23. */
  observe_char(&global_value);
  ASSERT(observed == 9);
  ASSERT(global_value == 9);

  global_pointee = 10;
  observe_pointer(&global_pointer);
  modify_pointee(&global_pointer);
  ASSERT(observed_pointer == &global_pointee);
  ASSERT(global_pointer == &global_pointee);
  ASSERT(*global_pointer == 23);
}

void
test_static_const_globals(void)
{
  /* static_global_value stays 11; the pointer still targets global_pointee.
     Treating the pointee as const would reuse 10 after the write, not 23. */
  observe_char(&static_global_value);
  ASSERT(observed == 11);
  ASSERT(static_global_value == 11);

  global_pointee = 10;
  observe_pointer(&static_global_pointer);
  modify_pointee(&static_global_pointer);
  ASSERT(observed_pointer == &global_pointee);
  ASSERT(static_global_pointer == &global_pointee);
  ASSERT(*static_global_pointer == 23);
}

void
test_mutable_globals(void)
{
  /* The calls change the globals to 17 and &replacement; reusing 9 or
     &global_pointee would fail the corresponding assertion. */
  mutable_global_value = 9;
  replace_char(&mutable_global_value);
  ASSERT(mutable_global_value == 17);

  mutable_global_pointer = &global_pointee;
  replace_pointer(&mutable_global_pointer);
  ASSERT(mutable_global_pointer == &replacement);
}

static int
reread_const_lvalue(const int *view, int *alias)
{
  /* local must stay 10 while *view becomes 23: reusing either value for
     both operands would return 20 or 46 instead of 33. */
  const int local = *view;
  *alias = 23; // *alias could be *view.
  return local + *view;
}

static int
reread_const_pointer_lvalue(int *const *view, int **alias)
{
  /* local keeps the old address while *view becomes &replacement. Reusing
     either pointer value for both operands would wrongly compare equal. */
  int *const local = *view;
  *alias = &replacement; // *alias could be *view.
  return local == *view;
}

struct value_holder { int value; };

static int
reread_const_member(const struct value_holder *view, struct value_holder *alias)
{
  /* local must stay 10 while view->value becomes 23: reusing either value
     for both operands would return 20 or 46 instead of 33. */
  const int local = view->value;
  alias->value = 23; // alias->value could be view->value.
  return local + view->value;
}

static int
reread_const_element(const int *view, int *alias)
{
  /* local must stay 10 while view[1] becomes 23: reusing either value
     for both operands would return 20 or 46 instead of 33. */
  const int local = view[1];
  alias[1] = 23; // alias[1] could be view[1].
  return local + view[1];
}

void
test_const_access_paths(void)
{
  /* The const snapshots must survive writes through aliases, while fresh
     loads must see the updated integers or pointer. */
  int value = 10;
  ASSERT(reread_const_lvalue(&value, &value) == 33);
  ASSERT(value == 23);

  int *pointer = &value;
  ASSERT(!reread_const_pointer_lvalue(&pointer, &pointer));
  ASSERT(pointer == &replacement);

  struct value_holder object = {10};
  ASSERT(reread_const_member(&object, &object) == 33);
  ASSERT(object.value == 23);

  int array[2] = {0, 10};
  ASSERT(reread_const_element(array, array) == 33);
  ASSERT(array[1] == 23);
}
