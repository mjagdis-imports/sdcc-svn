/* Diagnose explicit null pointer constants in implicit pointer conversions.
   Warning 378 is disabled by default. */

#ifdef __SDCC
#pragma std_c2y
#else
#define _Optional
#define nullptr ((void *)0)
#endif
#define NULL ((void *)0)

typedef void Callback (void);
void accept (int *);
void accept_optional (_Optional int *);
void accept_callback (Callback *);
void accept_optional_callback (_Optional Callback *);

#ifdef TEST1
#define DEFAULT_DIAGNOSTIC
/* The warning is disabled by default, for all conversion contexts. */
int *global = nullptr;
Callback *global_callback = 0;
int *test (int condition, int *p)
{
  accept (condition ? p : nullptr);
  p = NULL;
  accept (p);
  accept (nullptr);
  accept_callback (NULL);
  return nullptr;
}
#endif

#ifndef DEFAULT_DIAGNOSTIC
#ifdef __SDCC
#pragma enable_warning 378
#endif
#endif

#ifdef TEST2
/* Object pointer assignments, initialisation, arguments and return. */
int *global = nullptr; /* WARNING(SDCC) */
_Optional int *optional_global = nullptr;
int **nested = 0; /* WARNING(SDCC) */
_Optional int **optional_inner_target = nullptr; /* WARNING(SDCC) */
int *test (void)
{
  int *p = 0; /* WARNING(SDCC) */
  p = NULL; /* WARNING(SDCC) */
  p = nullptr; /* WARNING(SDCC) */
  accept (0); /* WARNING(SDCC) */
  accept (NULL); /* WARNING(SDCC) */
  accept (nullptr); /* WARNING(SDCC) */
  accept_optional (nullptr);
  accept (p);
  return nullptr; /* WARNING(SDCC) */
}
#endif

#ifdef TEST3
/* Function pointer conversions require the same diagnostics. */
Callback *global_callback = nullptr; /* WARNING(SDCC) */
_Optional Callback *optional_callback = nullptr;
Callback *test (void)
{
  Callback *callback = NULL; /* WARNING(SDCC) */
  callback = 0; /* WARNING(SDCC) */
  callback = nullptr; /* WARNING(SDCC) */
  accept_callback (0); /* WARNING(SDCC) */
  accept_callback (NULL); /* WARNING(SDCC) */
  accept_callback (nullptr); /* WARNING(SDCC) */
  accept_optional_callback (nullptr);
  accept_callback (callback);
  return nullptr; /* WARNING(SDCC) */
}
#endif

#ifdef TEST4
#ifdef __SDCC
#pragma disable_warning 110 /* The test deliberately creates dead conditional expressions. */
#endif
/* Warn in either conditional operand, including arrays and function pointers. */
void test (int condition, int *p, _Optional int *optional, Callback *fp,
           _Optional Callback *optional_fp)
{
  int array[1];
  p = condition ? p : nullptr; /* WARNING(SDCC) */
  p = condition ? nullptr : p; /* WARNING(SDCC) */
  p = condition ? array : 0; /* WARNING(SDCC) */
  fp = condition ? fp : nullptr; /* WARNING(SDCC) */
  fp = condition ? nullptr : fp; /* WARNING(SDCC) */
  optional = condition ? optional : nullptr;
  optional_fp = condition ? optional_fp : nullptr;
}
#endif

#ifdef TEST5
/* Static and aggregate initialisers must warn once per explicit value. */
struct S { int *p; Callback *callback; };
struct S global = {
  nullptr, /* WARNING(SDCC) */
  nullptr /* WARNING(SDCC) */
};
int *array[] = {
  0, /* WARNING(SDCC) */
  NULL /* WARNING(SDCC) */
};
void test (void)
{
  static int *p = 0; /* WARNING(SDCC) */
  static Callback *callback = nullptr; /* WARNING(SDCC) */
  struct S local = {
    nullptr, /* WARNING(SDCC) */
    nullptr /* WARNING(SDCC) */
  };
  accept (p);
  accept_callback (callback);
  accept (local.p);
}
#endif

#ifdef TEST6
/* Optional targets, explicit casts and comparisons stay quiet. */
int *explicit_global = (int *)nullptr;
Callback *explicit_callback = (Callback *)0;
_Optional int *optional = NULL;
int *_Optional *optional_outer_target = nullptr;
int *uninitialised;
void test (int *p, Callback *callback)
{
  _Optional int *local = nullptr;
  p = (int *)nullptr;
  callback = (Callback *)NULL;
  accept ((int *)0);
  accept_callback ((Callback *)nullptr);
  accept_optional (0);
  accept_optional_callback (NULL);
  (void)(p == nullptr);
  (void)(callback != nullptr);
  (void)local;
}
_Optional int *return_optional (void) { return nullptr; }
_Optional Callback *return_optional_callback (void) { return nullptr; }
#ifdef __SDCC
static_assert (_Generic (nullptr, void *: 0, default: 1));
#endif
#endif

#ifdef TEST7
/* Existing suppression also applies to an enabled warning. */
#ifdef __SDCC
#pragma disable_warning 378
#endif
int *test (void)
{
  accept (nullptr);
  accept_callback (NULL);
  return nullptr;
}
#endif

#ifdef TEST8
/* Integer constant expressions, including casts, are null pointer constants. */
void test (void)
{
  accept (1 - 1); /* WARNING(SDCC) */
  accept ((int)0); /* WARNING(SDCC) */
  accept_callback (1 - 1); /* WARNING(SDCC) */
  accept_optional (1 - 1);
}
#endif

#ifdef TEST9
#ifdef __SDCC
/* A constexpr integer zero is also an integer constant expression. */
constexpr int zero = 0;
void test (void)
{
  accept (zero); /* WARNING(SDCC) */
}
#endif

#endif

#ifdef TEST10
/* A non-optional pointer arm can flow into an optional destination. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
_Optional int *test (int condition, int other, int *p, int *q)
{
  _Optional int *initial = condition ? p : nullptr;
  initial = condition ? NULL : p;
  initial = condition ? p : 0;
  initial = condition ? p : (other ? q : nullptr);
  initial = condition ? (other ? nullptr : p) : q;
  initial = ((void)condition, condition ? p : nullptr);
  accept_optional (condition ? p : NULL);
  struct { _Optional int *p; } fields = { condition ? p : nullptr };
  accept_optional (fields.p);
  accept_optional (initial);
  return condition ? p : nullptr;
}
#endif

#ifdef TEST11
/* Preserve warnings at non-optional destinations, including nested arms. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
int *test (int condition, int other, int *p, int *q)
{
  int *initial = condition ? p : nullptr; /* WARNING(SDCC) */
  initial = condition ? NULL : p; /* WARNING(SDCC) */
  initial = condition ? p : 0; /* WARNING(SDCC) */
  initial = condition ? p : (other ? q : nullptr); /* WARNING(SDCC) */
  initial = condition ? (other ? nullptr : p) : q; /* WARNING(SDCC) */
  initial = ((void)condition, condition ? p : nullptr); /* WARNING(SDCC) */
  accept (condition ? p : NULL); /* WARNING(SDCC) */
  struct { int *p; } fields = { condition ? p : nullptr }; /* WARNING(SDCC) */
  accept (fields.p);
  accept (initial);
  return condition ? nullptr : p; /* WARNING(SDCC) */
}
#endif

#ifdef TEST12
/* Optional callback destinations also permit null conditional arms. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
_Optional Callback *test (int condition, Callback *callback)
{
  _Optional Callback *initial = condition ? callback : nullptr;
  initial = condition ? nullptr : callback;
  accept_optional_callback (condition ? callback : NULL);
  accept_optional_callback (initial);
  return condition ? callback : nullptr;
}
#endif

#ifdef TEST13
/* Non-optional callback destinations must still warn. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
Callback *test (int condition, Callback *callback)
{
  Callback *initial = condition ? callback : nullptr; /* WARNING(SDCC) */
  initial = condition ? nullptr : callback; /* WARNING(SDCC) */
  accept_callback (condition ? callback : NULL); /* WARNING(SDCC) */
  accept_callback (initial);
  return condition ? callback : nullptr; /* WARNING(SDCC) */
}
#endif

#ifdef TEST14
/* An explicit pointer cast suppresses null-arm diagnostics, even if the
   cast is removed because the conditional already has the same type. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
void test (int condition, int other, int *p)
{
  int *initial = condition ? p : (int *)nullptr;
  initial = (int *)(condition ? p : nullptr);
  initial = condition ? (int *)(other ? p : nullptr) : p;
  initial = condition ? (int *)nullptr : (other ? p : nullptr); /* WARNING(SDCC) */
  accept ((int *)(condition ? p : NULL));
  accept_callback (condition ? (Callback *)0 : (Callback *)nullptr);
  accept (initial);
}
#endif

#ifdef TEST15
/* Standalone conditionals and null comparisons impose no non-optional target. */
#ifdef __SDCC
#pragma disable_warning 110
#endif
void test (int condition, int *p)
{
  (void)(condition ? p : nullptr);
  (void)((condition ? p : NULL) == nullptr);
  (void)((condition ? nullptr : p) != nullptr);
  accept (((void)(condition ? p : nullptr), p));
}
#endif

#ifdef TEST16
#ifdef __SDCC
/* The diagnostic must not change the conditional-expression types. */
void test (int condition, int *p, _Optional int *optional)
{
  static_assert (_Generic (condition ? p : nullptr, int *: 1, default: 0));
  static_assert (_Generic (condition ? NULL : p, int *: 1, default: 0));
  static_assert (_Generic (condition ? optional : nullptr,
                           _Optional int *: 1, default: 0));
}
#endif
#endif

#ifdef TEST17
#ifdef __SDCC
#pragma disable_warning 110
#endif
/* Generic selection follows only the selected expression. */
void test (int condition, int other, int *p, int *q)
{
  accept (_Generic (condition, int: condition ? p : nullptr)); /* WARNING(SDCC) */
  accept (_Generic (condition, int: p, default: condition ? p : nullptr));
  // Separate lines keep SDCC's identical-message coalescing from merging arms.
  accept (condition ? (other ? p : nullptr) /* WARNING(SDCC) */
                    : (other ? q : nullptr)); /* WARNING(SDCC) */
}
#endif

#ifdef TEST18
/* A nullptr_t result needs one conversion, not one per operand. */
int *test (int condition)
{
  return condition ? nullptr : nullptr; /* WARNING(SDCC) */
}
#endif

#ifdef TEST19
/* Numbered pragmas affect declarations at their point of appearance. */
int *enabled = nullptr; /* WARNING(SDCC) */
#ifdef __SDCC
#pragma disable_warning 378
#endif
int *disabled = nullptr;
#ifdef __SDCC
#pragma enable_warning 378
#endif
int *enabled_again = nullptr; /* WARNING(SDCC) */
#ifdef __SDCC
#pragma disable_warning 378
#endif
/* A later pragma must not change the diagnostics for earlier declarations. */
#endif

#ifdef TEST20
/* Re-enabling must not diagnose an earlier disabled initialiser at emission. */
#ifdef __SDCC
#pragma disable_warning 378
#endif
int *quiet = nullptr;
int *quiet_expression = 1 - 1;
int *quiet_conditional = 1 ? (int *)0 : nullptr;
#ifdef __SDCC
#pragma enable_warning 378
#endif
int *loud = nullptr; /* WARNING(SDCC) */
#endif

#ifdef TEST21
/* Explicit pointer values must not become null pointer constants by folding. */
const int *qualified_void = (const void *)0;
volatile int *volatile_void = (volatile void *)0;
#ifdef __SDCC
#pragma disable_warning 196 /* Qualifier loss is separate from null constants. */
#endif
void *optional_void = (_Optional void *)0;
int *pointer_cast_chain = (void *)(int *)0;
int *nullptr_cast = (void *)nullptr;
#ifdef __SDCC
#pragma disable_warning 141 /* Pointer-to-int casts need not have equal widths. */
int *integer_cast_chain = (void *)(unsigned int)(int *)0;
#endif
int *ordinary_void = (void *)0; /* WARNING(SDCC) */
int *folded_void = (void *)(1 - 1); /* WARNING(SDCC) */
void test (void)
{
  accept ((void *)(int *)0);
  accept ((void *)nullptr);
  accept ((void *)0); /* WARNING(SDCC) */
}
#endif

#ifdef TEST22
/* Inherited initialisers are not diagnosed again at redeclarations. */
int *once = nullptr; /* WARNING(SDCC) */
extern int *once;
int *once;
#endif

#ifdef TEST23
#ifdef __SDCC
#pragma disable_warning 110
#endif
/* A constant condition must not discard null-arm diagnostic information. */
int *test (int *p)
{
  int *first = 1 ? p : nullptr; /* WARNING(SDCC) */
  first = 0 ? nullptr : p; /* WARNING(SDCC) */
  accept (1 ? p : nullptr); /* WARNING(SDCC) */
  accept_optional (1 ? p : nullptr);
  _Optional int *optional = 0 ? nullptr : p;
  first = (int *)(1 ? p : nullptr);
  accept_optional (optional);
  accept (first);
  return 1 ? p : nullptr; /* WARNING(SDCC) */
}
#endif

#ifdef TEST24
#ifdef __SDCC
#pragma disable_warning 110
/* The omitted-middle conditional has the same destination-aware behaviour. */
int *provide (void);
void test (int *p)
{
  accept (p ?: nullptr); /* WARNING(SDCC) */
  accept_optional (p ?: nullptr);
  accept (provide () ?: nullptr); /* WARNING(SDCC) */
  (void)(p ?: nullptr);
  (void)((p ?: nullptr) == nullptr);
}
#endif
#endif

#ifdef TEST25
/* Explicit null values in aggregates still use the conversion diagnostic. */
int *explicit_null = nullptr; /* WARNING(SDCC) */
int *explicit_array[1] = {0}; /* WARNING(SDCC) */
union Choice { int *p; int n; } explicit_union = {.p = nullptr}; /* WARNING(SDCC) */
#endif

#ifdef TEST26
#pragma enable_warning 378

void *alloc (unsigned);

static inline void *
wrap (unsigned size)
{
  return alloc (size);
}

int *
pointer_initialiser_calls_inline (void)
{
  int *p = wrap (2);
  return p;
}
#endif
