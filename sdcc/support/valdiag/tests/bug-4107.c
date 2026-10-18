/* Equivalent function-pointer associations must be diagnosed as duplicates. */
#ifdef SDCC
#pragma std_c11
#endif

typedef int function_type (int);
typedef function_type *pointer_type;
int f (int);
pointer_type pf;

#ifdef TEST1
int duplicate_pointer (void)
{
  return _Generic (pf, function_type *: 1, pointer_type: 2); /* ERROR */
}
#endif

#ifdef TEST2
int duplicate_address (void)
{
  return _Generic (&f, function_type *: 1, pointer_type: 2); /* ERROR */
}
#endif

#ifdef TEST3
/* Preservation: these associations already match before the fix. */
int duplicate_designator (void)
{
  return _Generic (f, function_type *: 1, int (*)(int): 2); /* ERROR */
}
#endif

