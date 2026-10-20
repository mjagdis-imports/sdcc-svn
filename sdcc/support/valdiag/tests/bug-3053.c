/* Bug #3053: a DS390 AOP_STR assertion with an invalid va_list pointer.
   Do not execute this source: b is advanced beyond its one-element array. */
#include <stdarg.h>

#ifdef TEST1C99
#ifdef __SDCC_ds390
int g(va_list ap);

void f(const char *s, ...)
{
  va_list *a[1], **b = a;
  g(**b);
  ++b;
  g(**b); /* WARNING (SDCC_ds390) */
}
#else
void f(void)
{
}
#endif
#endif
