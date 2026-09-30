/* N3886 6.7.7.4p1: a function declarator shall not specify a function return type. */
typedef int function_type(void);

#ifdef TEST1
function_type returns_function(void); /* ERROR */
#endif

#ifdef TEST2
function_type returns_function(void) /* ERROR */
{
  for (;;)
    ;
}
#endif

#ifdef TEST3
function_type *returns_pointer_to_function(void);
#endif

#ifdef TEST4
function_type *returns_pointer_to_function(void)
{
  return 0;
}
#endif

#ifdef TEST5
void test(void)
{
  (void)sizeof(int (*)(void)(void)); /* ERROR */
}
#endif

#ifdef TEST6
void test(void)
{
  (void)sizeof(int (*(*)(void))(void));
}
#endif
