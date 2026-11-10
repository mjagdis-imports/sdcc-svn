/* bug-3960.c

   Function calls and assignment operators do not produce lvalues.  The
   non-lvalue property of a structure expression is inherited by its members.
 */

int f(int p);
char g(int p);
struct S
{
  int x;
};
struct S h(void);
void consume(struct S);

#ifdef TEST1
void test(void)
{
  &f(0); // 'lvalue' required for 'address of' operation /* ERROR */
}
#endif

#ifdef TEST2
void test(void)
{
  int *p = &f(0); // 'lvalue' required for 'address of' operation /* ERROR */
  (void)p;
}
#endif

#ifdef TEST3
void test(void)
{
  char *p = &g(0); // 'lvalue' required for 'address of' operation /* ERROR */
  (void)p;
}
#endif

#ifdef TEST4
void test(void)
{
  int (*fp)(int) = f;
  &fp(0); // 'lvalue' required for 'address of' operation /* ERROR */
}
#endif

#ifdef TEST5
void test(void)
{
  int i;
  &(const int)i; // 'lvalue' required for 'address of' operation /* ERROR */
}
#endif

#ifdef TEST6
void test(void)
{
  &h().x; // 'lvalue' required for 'address of' operation /* ERROR */
}
#endif

#ifdef TEST7
void test(void)
{
  h().x = 1; // 'lvalue' required for '=' operation /* ERROR */
}
#endif

#ifdef TEST8
void test(void)
{
  struct S s;
  int *p = &s.x;
  int *q = &((struct S){0}).x;
  (void)p;
  (void)q;
}
#endif

#ifdef TEST9
void test(void)
{
  int i;
  &(i = 1); // 'lvalue' required for 'address of' operation /* ERROR */
}
#endif

#ifdef TEST10
void test(void)
{
  struct S s;
  s = h();
  consume(s = h());
}
#endif
