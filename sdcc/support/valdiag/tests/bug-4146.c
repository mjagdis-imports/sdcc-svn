// _Optional analysis must exclude paths ending at a _Noreturn call.
#ifdef __SDCC
#pragma disable_warning 126
_Noreturn void fail(void);
void may_return(void);

// A null argument cannot reach the dereference after this call.
#ifdef TEST1
int guarded_call(_Optional int *p)
{
  if (!p)
    fail();
  return *p;
}
#endif

// The conditional expression is the form used by assert-like macros.
#ifdef TEST2
int guarded_conditional(_Optional int *p)
{
  p ? (void)0 : fail();
  return *p;
}
#endif

// An ordinary function may return, so the null path must still warn.
#ifdef TEST3
int returning_call(_Optional int *p)
{
  if (!p)
    may_return();
  return *p; /* WARNING(SDCC) */
}
#endif

// Exiting on non-null does not make the remaining null path safe.
#ifdef TEST4
int wrong_branch(_Optional int *p)
{
  if (p)
    fail();
  return *p; /* WARNING(SDCC) */
}
#endif

// An explicit return already prevents the null path reaching this access.
#ifdef TEST5
int guarded_return(_Optional int *p)
{
  if (!p)
    return 0;
  return *p;
}
#endif

// Replacing the checked pointer with null must still warn.
#ifdef TEST6
int replaced_pointer(_Optional int *p)
{
  if (!p)
    fail();
  p = 0;
  return *p; /* WARNING(SDCC) */
}
#endif
#else
typedef int optional_noreturn_sdcc_only;
#endif
