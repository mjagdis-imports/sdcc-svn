/* Increment and decrement require arithmetic or pointer operands. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

typedef int bug_4136_translation_unit;

#ifndef SKIP_C23
#ifdef TEST1
/* Postfix increment requires arithmetic or pointer type, not nullptr_t. */
void f(void)
{
  typeof_unqual(nullptr) n = nullptr;
  n++; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif

#ifdef TEST2
/* The same constraint applies to postfix decrement. */
void f(void)
{
  typeof_unqual(nullptr) n = nullptr;
  n--; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif

#ifdef TEST3
/* Prefix increment must reject nullptr_t too. */
void f(void)
{
  typeof_unqual(nullptr) n = nullptr;
  ++n; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif

#ifdef TEST4
/* Prefix decrement must reject nullptr_t too. */
void f(void)
{
  typeof_unqual(nullptr) n = nullptr;
  --n; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif

#ifdef TEST5
/* A parameter of nullptr_t is subject to the same operand constraint. */
void f(typeof_unqual(nullptr) n)
{
  ++n; /* ERROR(SDCC) */ /* IGNORE(GCC) */
}
#endif

#endif /* !SKIP_C23 */

#ifdef TEST6
/* Preservation: a structure operand is not arithmetic or a pointer. */
struct S { int x; };
void f(struct S s)
{
  s++; /* ERROR */
}
#endif

#ifdef TEST7
/* Valid cases: both forms remain permitted for arithmetic and pointer types. */
void f(int n, int *p, float v)
{
  n++;
  n--;
  ++n;
  --n;
  p++;
  p--;
  ++p;
  --p;
  v++;
  v--;
  ++v;
  --v;
  (void)n;
  (void)p;
  (void)v;
}
#endif

#ifdef TEST8
/* Preservation: an arithmetic operand must still be a modifiable lvalue. */
void f(void)
{
  const int n = 1;
  ++n; /* ERROR */
}
#endif
