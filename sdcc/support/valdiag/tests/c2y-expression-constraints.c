/* Focused constraint tests; each case is compiled separately. */
int audit_nonempty;

#ifdef TEST1
/* The left operand of . must have complete structure or union type. */
struct S;
extern struct S s;
void f(void)
{
  (void)s.x; /* ERROR */
}
#endif


#ifdef TEST3
/* Valid case: the structure type is complete. */
struct S { int x; };
struct S s;
int f(void)
{
  return s.x;
}
#endif
