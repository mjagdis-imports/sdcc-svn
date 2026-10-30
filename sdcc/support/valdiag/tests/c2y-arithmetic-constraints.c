/* Additional N3886 diagnostic cases. */
int audit_nonempty;

/* N3886 6.5.5p4: a floating expression cannot be cast to pointer. */
#ifdef TEST1
float floating;
int *test (void)
{
  return (int *)floating; /* ERROR */
}
#endif

/* N3886 6.5.6p2: multiplication requires arithmetic operands. */
#ifdef TEST2
int *pointer;
void test (void)
{
  pointer * 2; /* ERROR */
}
#endif

/* N3886 6.5.6p2: remainder requires integer operands. */
#ifdef TEST3
float floating;
void test (void)
{
  floating % 2; /* ERROR */
}
#endif

/* N3886 6.5.7p3: subtracting a structure from a pointer is invalid. */
#ifdef TEST4
int *pointer;
struct aggregate { int member; } object;
void test (void)
{
  pointer - object; /* ERROR */
}
#endif

/* Valid pointer addition, integer shift, and remainder. */
#ifdef TEST6
int array[2];
int test (void)
{
  return *(array + 1) + (8 >> 1) + (7 % 3);
}
#endif

/* N3886 6.5.17.3p2: *= requires arithmetic operands. */
#ifdef TEST7
int *pointer;
void test (void)
{
  pointer *= 2; /* ERROR */
}
#endif
