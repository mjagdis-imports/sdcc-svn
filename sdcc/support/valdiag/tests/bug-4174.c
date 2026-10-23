/* Bug #4174: warn for invalid pointer comparisons. */

typedef int bug_4174_translation_unit;

#ifdef TEST1
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST2
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST3
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p != q; /* WARNING */
}
#endif

#ifdef TEST4
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p != q; /* WARNING */
}
#endif

#ifdef TEST5
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST6
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST7
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST8
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST9
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST10
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST11
/* Different referenced integer types require a warning. */
int test (int *p, char *q)
{
  return p >= q; /* WARNING */
}
#endif

#ifdef TEST12
/* Reversing incompatible pointer operands must still warn. */
int test (char *p, int *q)
{
  return p >= q; /* WARNING */
}
#endif

#ifdef TEST13
/* Function pointers cannot be ordered. */
typedef int function_type (int);
int test (function_type *p, function_type *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST14
/* Valid case: void is an incomplete object type compatible with itself. */
int test (void *p, void *q)
{
  return p < q;
}
#endif

#ifdef TEST15
/* Function pointers cannot be ordered. */
typedef int function_type (int);
int test (function_type *p, function_type *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST16
/* Valid case: void is an incomplete object type compatible with itself. */
int test (void *p, void *q)
{
  return p > q;
}
#endif

#ifdef TEST17
/* Function pointers cannot be ordered. */
typedef int function_type (int);
int test (function_type *p, function_type *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST18
/* Valid case: void is an incomplete object type compatible with itself. */
int test (void *p, void *q)
{
  return p <= q;
}
#endif

#ifdef TEST19
/* Function pointers cannot be ordered. */
typedef int function_type (int);
int test (function_type *p, function_type *q)
{
  return p >= q; /* WARNING */
}
#endif

#ifdef TEST20
/* Valid case: void is an incomplete object type compatible with itself. */
int test (void *p, void *q)
{
  return p >= q;
}
#endif

#ifdef TEST21
/* Function return types must be compatible. */
typedef int int_function (void);
typedef char char_function (void);
int test (int_function *p, char_function *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST22
/* Function return types must be compatible. */
typedef int int_function (void);
typedef char char_function (void);
int test (int_function *p, char_function *q)
{
  return p != q; /* WARNING */
}
#endif

#ifdef TEST23
/* Different structure tags denote incompatible types. */
struct A { int member; };
struct B { int member; };
int test (struct A *p, struct B *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST24
/* Nested qualification must match. */
int test (int **p, const int **q)
{
  return p != q; /* WARNING */
}
#endif

#ifdef TEST25
/* Array operands undergo conversion before checking the referenced types. */
int integers[2];
char characters[2];
int test (void)
{
  return integers == characters; /* WARNING */
}
#endif

#ifdef TEST26
/* Function designators undergo conversion before checking compatibility. */
int first (int);
int second (char);
int test (void)
{
  return first != second; /* WARNING */
}
#endif

#ifdef TEST27
/* The object/void exception does not permit function-pointer equality. */
typedef int function_type (int);
int test (function_type *p, void *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST28
/* Optimisation of identical operands must not hide invalid function ordering. */
typedef int function_type (int);
int test (function_type *p)
{
  return p < p; /* WARNING */
}
#endif

#ifdef TEST29
/* Constant folding must not hide incompatible referenced types. */
int test (void)
{
  return (int *)0 == (char *)0; /* WARNING */
}
#endif

#ifdef TEST30
/* Void and int are incompatible referenced types for relational comparison. */
int test (void *p, int *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST31
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p == q;
}
#endif

#ifdef TEST32
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p != q;
}
#endif

#ifdef TEST33
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p < q;
}
#endif

#ifdef TEST34
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p > q;
}
#endif

#ifdef TEST35
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p <= q;
}
#endif

#ifdef TEST36
/* Valid case: immediate const and volatile qualifiers may differ. */
int test (const int *p, volatile int *q)
{
  return p >= q;
}
#endif

#ifdef TEST37
/* Valid case: object and qualified void pointers may be equal. */
int test (int *p, const void *q)
{
  return p == q;
}
#endif

#ifdef TEST38
/* Valid case: void and object pointers may be unequal. */
int test (void *p, int *q)
{
  return p != q;
}
#endif

#ifdef TEST39
/* Valid case: compatible function pointers may be equal. */
typedef int function_type (int);
int test (function_type *p, function_type *q)
{
  return p == q;
}
#endif

#ifdef TEST40
/* Valid case: compatible function designators may be unequal. */
int first (int);
int second (int);
int test (void)
{
  return first != second;
}
#endif

#ifdef TEST41
/* Valid case: a function pointer may be compared with a void-pointer null pointer constant. */
typedef int function_type (int);
int test (function_type *p)
{
  return p == (void *)0;
}
#endif

#ifdef TEST42
/* Valid case: an integer null pointer constant may precede a function pointer. */
typedef int function_type (int);
int test (function_type *p)
{
  return 0 != p;
}
#endif

#ifdef TEST43
/* Valid case: an integer null pointer constant may accompany a function pointer. */
typedef int function_type (int);
int test (function_type *p)
{
  return p != 0;
}
#endif

#ifdef TEST44
/* Valid case: pointers to arrays with known and unknown bounds are compatible. */
int test (int (*p)[], int (*q)[2])
{
  return p < q;
}
#endif

#ifdef TEST45
/* Valid case: qualification of a referenced pointer itself may differ. */
int test (int **p, int *const *q)
{
  return p == q;
}
#endif

#ifdef TEST46
/* Valid case: pointers to the same incomplete object type may be ordered. */
struct incomplete;
int test (struct incomplete *p, struct incomplete *q)
{
  return p < q;
}
#endif

#ifdef TEST47
/* Valid case: an array converts to a pointer to its element type. */
int integers[2];
int test (void)
{
  return integers < integers + 1;
}
#endif

#ifdef TEST48
/* Valid case: object address spaces retain the existing comparison rules. */
#ifdef __SDCC_mcs51
int test (int *p, __data int *q)
{
  return p == q;
}
#endif
#endif

#ifdef TEST49
/* Valid case: checking both optional pointers permits relational comparison. */
#ifdef __SDCC
int test (_Optional int *p, _Optional int *q)
{
  if (p && q)
    {
      return p < q;
    }
  return 0;
}
#endif
#endif

#ifdef TEST50
/* Valid case: nullptr may accompany an object pointer in C23 and later. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
int test (int *p)
{
  return p == nullptr;
}
#endif
#endif

#ifdef TEST51
/* Relational comparison requires compatible referenced types in either order. */
int test (int *p, void *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST52
/* Relational comparison requires compatible referenced types in either order. */
int test (void *p, int *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST53
/* Relational comparison requires compatible referenced types in either order. */
int test (int *p, void *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST54
/* Relational comparison requires compatible referenced types in either order. */
int test (void *p, int *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST55
/* Relational comparison requires compatible referenced types in either order. */
int test (int *p, void *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST56
/* Relational comparison requires compatible referenced types in either order. */
int test (void *p, int *q)
{
  return p >= q; /* WARNING */
}
#endif

#ifdef TEST57
/* Relational comparison requires compatible referenced types in either order. */
int test (int *p, void *q)
{
  return p >= q; /* WARNING */
}
#endif

#ifdef TEST58
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p == q;
}
#endif

#ifdef TEST59
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p != q;
}
#endif

#ifdef TEST60
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p < q;
}
#endif

#ifdef TEST61
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p > q;
}
#endif

#ifdef TEST62
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p <= q;
}
#endif

#ifdef TEST63
/* Valid case: qualification may differ for the same incomplete structure type. */
struct incomplete;
int test (const struct incomplete *p, volatile struct incomplete *q)
{
  return p >= q;
}
#endif

#ifdef TEST64
/* Valid case: qualification may differ for the incomplete void type. */
int test (const void *p, volatile void *q)
{
  return p < q;
}
#endif

#ifdef TEST65
/* Valid case: qualification may differ for the incomplete void type. */
int test (const void *p, volatile void *q)
{
  return p > q;
}
#endif

#ifdef TEST66
/* Valid case: qualification may differ for the incomplete void type. */
int test (const void *p, volatile void *q)
{
  return p <= q;
}
#endif

#ifdef TEST67
/* Valid case: qualification may differ for the incomplete void type. */
int test (const void *p, volatile void *q)
{
  return p >= q;
}
#endif

#ifdef TEST68
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p == q; /* WARNING */
}
#endif

#ifdef TEST69
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p != q; /* WARNING */
}
#endif

#ifdef TEST70
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p < q; /* WARNING */
}
#endif

#ifdef TEST71
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p > q; /* WARNING */
}
#endif

#ifdef TEST72
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p <= q; /* WARNING */
}
#endif

#ifdef TEST73
/* Distinct incomplete structure tags denote incompatible referenced types. */
struct first;
struct second;
int test (struct first *p, struct second *q)
{
  return p >= q; /* WARNING */
}
#endif
