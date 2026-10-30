/* Additional N3886 diagnostic cases. */
int audit_nonempty;

/* N3886 6.5.11p2: binary & requires integer operands. */
#ifdef TEST1
float floating;
void test (void)
{
  (void)(floating & 1); /* ERROR */
}
#endif

/* N3886 6.5.12p2: binary ^ requires integer operands. */
#ifdef TEST2
float floating;
void test (void)
{
  (void)(floating ^ 1); /* ERROR */
}
#endif

/* N3886 6.5.13p2: binary | requires integer operands. */
#ifdef TEST3
float floating;
void test (void)
{
  (void)(floating | 1); /* ERROR */
}
#endif

/* N3886 6.5.14p2: both operands of && have scalar type. */
#ifdef TEST4
struct aggregate { int member; } object;
void test (void)
{
  (void)(object && 1); /* ERROR */
}
#endif

/* N3886 6.5.15p2: both operands of || have scalar type. */
#ifdef TEST5
struct aggregate { int member; } object;
void test (void)
{
  (void)(object || 1); /* ERROR */
}
#endif

/* N3886 6.5.16p3: the result operands of ?: have compatible categories. */
#ifdef TEST6
struct aggregate { int member; } object;
void test (void)
{
  (void)(1 ? object : 2); /* ERROR */
}
#endif

/* Valid integer bitwise, scalar logical, and conditional expressions. */
#ifdef TEST7
int test (int value)
{
  return (value & 1) || (value ? 1 : 0);
}
#endif
