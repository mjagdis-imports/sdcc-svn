/* bug-4000.c

   typeof expressions must not retain storage-class information from their
   operands and must not provoke spurious diagnostics.
 */

#pragma std_c23

#ifdef TEST1
int f(void);

void test_function_call(void)
{
  typeof(f()) i = 3;
  i = 2;
}
#endif

#ifdef TEST2
extern const volatile int object;

void test_object(void)
{
  typeof(object) i = 3;
  (void)i;
}
#endif
