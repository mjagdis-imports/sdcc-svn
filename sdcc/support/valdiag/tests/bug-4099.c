/* N3886 6.7.7.4p1: a function declarator shall not specify an array return type. */
#ifdef TEST1
int returns_array(void)[3]; /* ERROR */
#endif

#ifdef TEST2
int returns_array(void)[3] /* ERROR */
{
}
#endif

#ifdef TEST3
int (*returns_pointer_to_array(void))[3];
#endif

#ifdef TEST4
int (*returns_pointer_to_array(void))[3]
{
  return 0;
}
#endif

#ifdef TEST5
void test(void)
{
  (void)sizeof(int (*)(void)[3]); /* ERROR */
}
#endif

#ifdef TEST6
void test(void)
{
  (void)sizeof(int (*(*)(void))[3]);
}
#endif

#ifdef TEST7
void test(void)
{
  (void)sizeof(int (*)(void)[]); /* ERROR */
}
#endif

#ifdef TEST8
void test(void)
{
  (void)sizeof(int (*(*)(void))[]);
}
#endif
