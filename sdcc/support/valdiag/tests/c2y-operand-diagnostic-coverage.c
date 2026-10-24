/* Compile each selected constraint case separately. */
int audit_nonempty;

#ifdef TEST1
/* An undeclared ordinary identifier is invalid. */
void f(void)
{
  (void)(missing); /* ERROR */
}
#endif

#ifdef TEST2
/* An undeclared identifier remains invalid in sizeof. */
void f(void)
{
  (void)(sizeof missing); /* ERROR */
}
#endif

#ifdef TEST3
/* An unselected association must still refer to declared identifiers. */
void f(void)
{
  (void)(_Generic(1, int: 0, default: missing)); /* ERROR */
}
#endif


#ifdef TEST5
/* Compound array literals obey excess-initialiser constraints. */
void f(void)
{
  (void)(int[1]){1, 2}; /* WARNING */
}
#endif

#ifdef TEST6
/* Compound structure literals obey excess-initialiser constraints. */
struct S { int x; };
void f(void)
{
  (void)(struct S){1, 2}; /* WARNING */
}
#endif

#ifdef TEST7
/* Valid case: a fixed-bound array literal has enough elements. */
void f(void)
{
  (void)(int[2]){1, 2};
}
#endif

#ifdef TEST8
/* Valid case: a declared identifier can be used. */
int f(int x)
{
  return x;
}
#endif
