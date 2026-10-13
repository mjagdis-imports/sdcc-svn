/* Each numbered block is a separate diagnostic check. */

#ifdef TEST1
/* The << count must have integer type, including floating zero; the baseline is silent. */
int test (int value)
{
  return value << 0.0; /* ERROR */
}
#endif

#ifdef TEST2
/* The << count must have integer type, including fractional floating literal; the baseline is silent. */
int test (int value)
{
  return value << 1.5; /* ERROR */
}
#endif

#ifdef TEST3
/* The << count must have integer type, including floating variable; the baseline is silent. */
int test (int value, float count)
{
  return value << count; /* ERROR */
}
#endif

#ifdef TEST4
/* The >> count must have integer type, including floating zero; the baseline is silent. */
int test (int value)
{
  return value >> 0.0; /* ERROR */
}
#endif

#ifdef TEST5
/* The >> count must have integer type, including fractional floating literal; the baseline is silent. */
int test (int value)
{
  return value >> 1.5; /* ERROR */
}
#endif

#ifdef TEST6
/* The >> count must have integer type, including floating variable; the baseline is silent. */
int test (int value, float count)
{
  return value >> count; /* ERROR */
}
#endif

#ifdef TEST7
/* The <<= count must have integer type, including floating zero; the baseline is silent. */
int test (int value)
{
  value <<= 0.0; /* ERROR */
  return value;
}
#endif

#ifdef TEST8
/* The <<= count must have integer type, including fractional floating literal; the baseline is silent. */
int test (int value)
{
  value <<= 1.5; /* ERROR */
  return value;
}
#endif

#ifdef TEST9
/* The <<= count must have integer type, including floating variable; the baseline is silent. */
int test (int value, float count)
{
  value <<= count; /* ERROR */
  return value;
}
#endif

#ifdef TEST10
/* The >>= count must have integer type, including floating zero; the baseline is silent. */
int test (int value)
{
  value >>= 0.0; /* ERROR */
  return value;
}
#endif

#ifdef TEST11
/* The >>= count must have integer type, including fractional floating literal; the baseline is silent. */
int test (int value)
{
  value >>= 1.5; /* ERROR */
  return value;
}
#endif

#ifdef TEST12
/* The >>= count must have integer type, including floating variable; the baseline is silent. */
int test (int value, float count)
{
  value >>= count; /* ERROR */
  return value;
}
#endif

#ifdef TEST13
/* A floating left operand of << is already diagnosed by the baseline. */
int test (float value)
{
  return value << 1; /* ERROR */
}
#endif

#ifdef TEST14
/* A floating left operand of >> is already diagnosed by the baseline. */
int test (float value)
{
  return value >> 1; /* ERROR */
}
#endif

#ifdef TEST15
/* Valid case: integer counts are accepted for both shifts and compound assignments. */
int test (unsigned int value, unsigned char count)
{
  value <<= count;
  value >>= count;
  return (value << 1) + (value >> 1);
}
#endif

#ifdef TEST16
/* Valid case: enum and integer bit-field counts have integer type. */
enum count { one = 1 };
struct counts { unsigned int count : 2; };
int test (unsigned int value, enum count count, struct counts *p)
{
  return (value << count) + (value >> p->count);
}
#endif
