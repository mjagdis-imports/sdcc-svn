/* Each selected case is compiled separately. */

int audit_nonempty;

#ifdef TEST1
/* An array expression cannot initialise another array. */
int f(void)
{
  int b[2] = {1, 2};
  int a[2] = b; /* ERROR */
  return a[0];
}
#endif

#ifdef TEST2
/* A pointer expression is not an array initialiser. */
int f(char *p)
{
  char a[4] = p; /* ERROR */
  return a[0];
}
#endif

#ifdef TEST3
/* Valid case: a brace-enclosed list initialises the elements. */
int f(void)
{
  int a[2] = {1, 2};
  return a[0];
}
#endif

#ifdef TEST4
/* Valid case: a string literal initialises a character array. */
int f(void)
{
  char a[] = "abc";
  return a[0];
}
#endif
