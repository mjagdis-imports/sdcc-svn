#ifdef TEST1
int *foo(_Optional int *poi)
{
  int *pi = &*poi; /* WARNING */
  return pi;
}
#endif

#ifdef TEST2
int *direct(_Optional int *p)
{
  return &*p; /* WARNING */
}
#endif

#ifdef TEST3
int *later_guard(_Optional int *p)
{
  int *q = &*p; /* WARNING */
  if (q)
    return q;
  return 0;
}
#endif

#ifdef TEST4
int *guarded(_Optional int *p)
{
  if (p)
    {
      int *q = &*p;
      return q;
    }
  return 0;
}
#endif

#ifdef TEST5
void unevaluated(_Optional int *p)
{
  (void)sizeof(&*p);
}
#endif

#ifdef TEST6
int *plain(int *p)
{
  int *q = &*p;
  return q;
}
#endif

#ifdef TEST7
int *known_object(void)
{
  static int i;
  _Optional int *p = &i;
  int *q = &*p;
  return q;
}
#endif

#ifdef TEST8
#pragma disable_warning 126
int *dead_branch(_Optional int *p)
{
  if (0)
    {
      int *q = &*p;
      return q;
    }
  return 0;
}
#endif
