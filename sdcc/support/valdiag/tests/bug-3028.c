#pragma std_c11

#if __has_reentrant || defined(__SDCC_pdk13) || defined(__SDCC_pdk14) || \
    defined(__SDCC_pdk15)
typedef int Function(int) __reentrant;
#else
typedef int Function(int);
#endif

#ifdef TEST1
int function_parameter_first(Function function);
int function_parameter_first(Function *function);
int pointer_parameter_first(Function *function);
int pointer_parameter_first(Function function);
#endif

#ifdef TEST2
typedef int EvaluationFunction(Function);
typedef int (*EvaluationPointer)(Function *);
_Static_assert(_Generic((EvaluationFunction *)0,
                        EvaluationPointer: 1, default: 0),
               "adjusted unnamed parameter");
#endif

#ifdef TEST3
int guarded(_Optional Function function)
{
  if (function)
    return function(4);
  return 0;
}
#endif

#ifdef TEST4
int unguarded(_Optional Function function)
{
  return function(4); /* WARNING */
}
#endif

#ifdef TEST5
/* This is the original #3028 example, without a function typedef. */
void original(int (p)(void))
{
  p();
}
#endif
