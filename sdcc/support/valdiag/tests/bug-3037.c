/* Bug #3037: inline extern references and duplicate parameter names.
   Both reported samples are compiled without being executed. */
#ifdef TEST1C99
static inline void f1(int x)
{
  extern int w;
  w; /* WARNING */
}

void f2(void)
{
  f1(0);
}
#endif

#ifdef TEST2C99
void foo(int d) /* ERROR (SDCC) */ /* IGNORE (GCC) */
{
  int d; /* ERROR */
}
#endif
