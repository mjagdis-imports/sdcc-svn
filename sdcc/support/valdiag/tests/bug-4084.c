/* N3886 6.5.16p2: the first operand of ?: has scalar type. */
#ifdef TEST1
struct aggregate
{
  int member;
} object;
void test(void)
{
  (void)(object ? 1 : 2); /* ERROR */
}
#endif

#ifdef TEST2
/* A union is not scalar either. */
union aggregate
{
  int member;
} object;
void test(void)
{
  (void)(object ? 1 : 2); /* ERROR */
}
#endif

#ifdef TEST3
/* A void expression is not a scalar condition. */
void action(void);
void test(void)
{
  (void)(action() ? 1 : 2); /* ERROR */
}
#endif

#ifdef TEST4
/* Valid cases: arithmetic and pointer conditions remain accepted. */
int test(int integer, float real, int *pointer)
{
  return (integer ? 1 : 2) + (real ? 1 : 2) + (pointer ? 1 : 2);
}
#endif

#ifdef TEST5
/* Valid cases: arrays and function designators convert to pointers. */
int object[2];
void action(void);
int test(void)
{
  return (object ? 1 : 2) + (action ? 1 : 2); /* IGNORE(GCC) */
}
#endif

#ifdef TEST6
/* Valid case: nullptr_t is a scalar type in C23. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif
#ifndef SKIP_C23
int test(void)
{
  return nullptr ? 1 : 2;
}
#else
void dummy(void);
#endif
#endif
