/* Function parameters are adjusted to pointers to functions. */
#include <testfwk.h>

typedef int Function(int) __reentrant;
typedef int EvaluationFunction(Function) __reentrant;

static int increment(int value) __reentrant
{
  return value + 1;
}

static int evaluate_at_four(Function *function) __reentrant;
static int evaluate_at_four(Function function) __reentrant
{
  ASSERT(sizeof(function) == sizeof(Function *));
  return function(4);
}

static int evaluate_at_eight(Function) __reentrant;
static int evaluate_at_eight(Function function) __reentrant
{
  return function(8);
}

static unsigned char calls;

static int mark_called(void) __reentrant
{
  ++calls;
  return 1;
}

/* The named function parameter from #3028 must denote a pointer object. */
static void invoke(int (callback)(void) __reentrant) __reentrant
{
  callback();
}

void testBug(void)
{
  EvaluationFunction *evaluation_function = evaluate_at_four;
  calls = 0;
  invoke(mark_called);
  ASSERT(calls == 1);
  ASSERT(evaluate_at_four(increment) == 5);
  ASSERT(evaluate_at_eight(increment) == 9);
  ASSERT(evaluation_function(increment) == 5);
}
