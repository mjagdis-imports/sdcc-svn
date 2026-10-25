/* Invalid case expressions must be diagnosed without a compiler crash. */
#ifdef __SDCC
#pragma std_c2y
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 202311L
#define SKIP_C2Y
#endif

void unrelated (void);
void matched (void);

#ifdef TEST1
/* An evaluated comma operator is not an integer constant expression. */
void test (int x)
{
  switch (x)
    {
    case (0, 1): /* ERROR */
      break;
    }
}
#endif

#ifdef TEST2
/* Exercise insertion after an existing case value as well. */
void test (int x)
{
  switch (x)
    {
    case 2:
      break;
    case (0, 1): /* ERROR */
      break;
    }
}
#endif

#ifdef TEST3
/* A comma in an unevaluated conditional operand is permitted.
   GCC may still warn that its left operand has no effect. */
void test (int x)
{
  switch (x)
    {
    case 1 ? 3 : (0, 1): /* IGNORE(GCC) */
      matched ();
      break;
    case 0:
      matched ();
      break;
    }
}
#endif

#ifdef TEST4
/* An ordinary integer constant expression remains valid. */
void test (int x)
{
  switch (x)
    {
    case 1 + 2:
      matched ();
      break;
    }
}
#endif

#ifndef SKIP_C2Y

#ifdef TEST5
/* Range endpoints require actual constant values too. */
void test (int x)
{
  switch (x)
    {
    case (0, 1) ... 3: /* ERROR(SDCC) */ /* IGNORE(GCC) */
      break;
    }
}
#endif

#ifdef TEST6
/* Check the upper endpoint independently. */
void test (int x)
{
  switch (x)
    {
    case 1 ... (2, 3): /* ERROR(SDCC) */ /* IGNORE(GCC) */
      break;
    }
}
#endif

#ifdef TEST7
/* A literal lower endpoint still has to have integer type. */
void test (int x)
{
  switch (x)
    {
    case 1.5 ... 3: /* ERROR(SDCC) */ /* IGNORE(GCC) */
      break;
    }
}
#endif

#ifdef TEST8
/* Check the upper endpoint's type independently. */
void test (int x)
{
  switch (x)
    {
    case 1 ... 3.5: /* ERROR(SDCC) */ /* IGNORE(GCC) */
      break;
    }
}
#endif

#ifdef TEST9
/* Valid C2Y integer ranges must retain their labels. */
void test (int x)
{
  switch (x)
    {
    case 1 ... 3:
      matched ();
      break;
    }
}
#endif

#endif /* !SKIP_C2Y */
