/* Relational checks must use the source pointer qualification. */
#ifdef __SDCC

#ifdef TEST1
/* Checking the optional operand suffices in either operand position. */
int compare(_Optional char *p, char *q)
{
  if (!p)
    {
      return 0;
    }

  int result = p < q;
  result += p > q;
  result += p <= q;
  result += p >= q;
  result += q < p;
  result += q > p;
  result += q <= p;
  result += q >= p;
  return result;
}
#endif

#ifdef TEST2
/* An unchecked optional operand still requires a diagnostic. */
int compare(_Optional char *p, char *q)
{
  int result = p < q; /* WARNING(SDCC) */
  result += p > q; /* WARNING(SDCC) */
  result += p <= q; /* WARNING(SDCC) */
  result += p >= q; /* WARNING(SDCC) */
  result += q < p; /* WARNING(SDCC) */
  result += q > p; /* WARNING(SDCC) */
  result += q <= p; /* WARNING(SDCC) */
  result += q >= p; /* WARNING(SDCC) */
  return result;
}
#endif

#ifdef TEST3
/* Checking one optional operand does not establish the other's value. */
int compare(_Optional char *p, _Optional char *q)
{
  if (p)
    {
      int result = p >= q; /* WARNING(SDCC) */
      result += q >= p; /* WARNING(SDCC) */
      return result;
    }
  return 0;
}
#endif

#ifdef TEST4
/* Both optional operands can be checked before ordering them. */
int compare(_Optional char *p, _Optional char *q)
{
  if (p && q)
    {
      int result = p >= q;
      result += q >= p;
      return result;
    }
  return 0;
}
#endif

#ifdef TEST5
/* Equality comparisons permit null even without a preceding check. */
int compare(_Optional char *p, _Optional char *q)
{
  return (p == q) + (p != q);
}
#endif

#ifdef TEST6
/* An explicit cast to an optional target type still requires a check. */
int compare(char *p, char *q)
{
  int result = (_Optional char *)p >= q; /* WARNING(SDCC) */
  result += q >= (_Optional char *)p; /* WARNING(SDCC) */
  return result;
}
#endif

#else
typedef int bug_4169_requires_sdcc;
#endif
