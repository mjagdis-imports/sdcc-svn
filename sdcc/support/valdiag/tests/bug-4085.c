/* N3886 6.5.7p2: the other operand of pointer addition is integer. */
#ifdef TEST1
int *pointer;
float floating;
int *test (void)
{
  return pointer + floating; /* ERROR */
}
#endif

/* N3886 6.5.7p3: pointer subtraction requires compatible pointed-to types. */
#ifdef TEST2
int *pointer;
char *other;
long test (void)
{
  return pointer - other; /* ERROR */
}
#endif

/* N3886 6.5.7p3: pointed-to object types must also be complete. */
#ifdef TEST3
struct incomplete;
struct incomplete *left, *right;
long test (void)
{
  return left - right; /* ERROR */
}
#endif

/* Array operands have not yet undergone array-to-pointer conversion in the
   decorated AST. */
#ifdef TEST4
int array[2];
float floating;
int *test (void)
{
  return array + floating; /* ERROR */
}
#endif

/* N3886 6.5.7p3 does not permit an arithmetic left operand and pointer right
   operand. */
#ifdef TEST5
int value;
int *pointer;
long test (void)
{
  return value - pointer; /* ERROR */
}
#endif

/* Check compatible element types before array-to-pointer conversion has been
   represented in the decorated AST. */
#ifdef TEST6
int left[2];
char right[2];
long test (void)
{
  return left - right; /* ERROR */
}
#endif

/* Preserve SDCC's extension permitting arithmetic on void pointers. */
#ifdef TEST7
void *pointer;
void *test (void)
{
  return pointer + 1; /* WARNING */
}
#endif
