/* N3886 constraints: invalid cases require a diagnostic. */
#ifdef __GNUC__
#pragma GCC diagnostic error "-Wimplicit-int"
#endif
int audit_nonempty;

#ifdef TEST1
/* A member declaration needs a type specifier. */
struct S { const x; }; /* ERROR */
#endif

#ifdef TEST2
/* A type name needs a type specifier. */
int f(void)
{
  return sizeof(const); /* ERROR */
}
#endif

#ifdef TEST3
/* void cannot be combined with signed. */
signed void f(void); /* ERROR */
#endif

#ifdef TEST4
/* _Bool cannot be combined with unsigned. */
unsigned _Bool x; /* ERROR */
#endif

#ifdef TEST5
/* char cannot be combined with int. */
char int x; /* ERROR */
#endif

#ifdef TEST6
/* Three long specifiers are not permitted. */
long long long x; /* ERROR */
#endif

#ifdef TEST7
/* Valid case: unsigned long long is a permitted combination. */
unsigned long long x;
#endif
