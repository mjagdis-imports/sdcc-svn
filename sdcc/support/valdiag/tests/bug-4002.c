/* bug-4002.c

   Subtracting zero from a pointer to an _Optional-qualified type lost the
   pointer-arithmetic diagnostic when the subtraction was optimized away.
 */

#pragma disable_warning 196
#pragma disable_warning 355

#ifdef TEST1
int *minus_zero (_Optional int *p)
{
  return p - 0; /* WARNING */
}
#endif

#ifdef TEST2
int *known_nonnull (_Optional int *p)
{
  if (p)
    return p - 0;
  return 0;
}
#endif

#ifdef TEST3
int *intermediate_result (_Optional int *p)
{
  int *result = p - 0; /* WARNING */
  return result;
}
#endif
