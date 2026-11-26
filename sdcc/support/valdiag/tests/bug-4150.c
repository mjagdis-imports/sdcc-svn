/* Moving a for declaration earlier must not extend its scope. */

#ifdef TEST1
void after_loop (void)
{
  for (int i = 0; i < 1; ++i)
    {
    }
  i = 2; /* ERROR */
}
#endif

#ifdef TEST2
void reuse_name (void)
{
  for (int i = 0; i < 1; ++i)
    {
    }
  /* The name can be reused after the loop. */
  int i = 2;
  (void)i;
}
#endif
