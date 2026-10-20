/* Bug #2860: identifier-list declarations must not crash the parser.
   These are compilation tests; the function with no return is never called. */
#ifdef TEST1C99
extern void *f(g); /* WARNING (GCC) */
#endif

#ifdef TEST2C99
extern void *h(g)
int g;
{
} /* WARNING */
#endif
