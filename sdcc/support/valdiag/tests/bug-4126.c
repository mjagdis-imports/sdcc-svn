/* Invalid pointer combinations must be diagnosed. */
#ifdef __SDCC
#pragma std_c23
#elif __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

/* Some cases are specific to SDCC or unavailable in older host modes. */
typedef int bug_4126_translation_unit;

#ifdef TEST1
/* int * and char * have incompatible referenced types; the baseline is silent. */
void *test (int condition, int *p, char *q)
{
  return condition ? p : q; /* WARNING */
}
#endif

#ifdef TEST2
/* Reversing the operands must not hide incompatible referenced types. */
void *test (int condition, char *p, int *q)
{
  return condition ? p : q; /* WARNING */
}
#endif

#ifdef TEST3
/* Distinct structure tags denote incompatible types, even with identical members. */
struct A { int member; };
struct B { int member; };
void *test (int condition, struct A *p, struct B *q)
{
  return condition ? p : q; /* WARNING */
}
#endif

#ifdef TEST4
/* Array operands decay before checking their incompatible element types. */
int integers[2];
char characters[2];
void *test (int condition)
{
  return condition ? integers : characters; /* WARNING */
}
#endif

#ifdef TEST5
/* Array and pointer operands must be checked in either order. */
int integers[2];
void *test (int condition, char *p)
{
  return condition ? p : integers; /* WARNING */
}
#endif

#ifdef TEST6
/* Pointed-to pointer qualification is nested and must match. */
void *test (int condition, int **p, const int **q)
{
  return condition ? p : q; /* WARNING */
}
#endif

#ifdef TEST7
/* Function pointers with incompatible return types need a diagnostic. */
typedef int int_function (void);
typedef char char_function (void);
int test (int condition, int_function *p, char_function *q)
{
  return (condition ? p : q) != 0; /* WARNING */
}
#endif

#ifdef TEST8
/* Even an unselected operand of a constant conditional must satisfy the constraints. */
void *test (int *p, char *q)
{
  return 1 ? p : q; /* WARNING */
}
#endif

#ifdef TEST9
/* Valid case: immediate const and volatile qualification may be combined. */
const volatile int *test (int condition, const int *p, volatile int *q)
{
  return condition ? p : q;
}
#endif

#ifdef TEST10
/* Valid case: an object pointer may be combined with void * in either order. */
void *test (int condition, int *p, void *q)
{
  void *first = condition ? p : q;
  return condition ? q : first;
}
#endif

#ifdef TEST11
/* Valid case: integer and void-pointer null pointer constants may accompany an object pointer. */
int *test (int condition, int *p)
{
  int *first = condition ? p : 0;
  return condition ? (void *)0 : first;
}
#endif

#ifdef TEST12
/* Valid case: compatible array and pointer operands undergo array decay. */
int integers[2];
const int *test (int condition, const int *p)
{
  return condition ? integers : p;
}
#endif

#ifdef TEST13
/* Valid case: pointers to arrays of unknown and known size are compatible. */
void *test (int condition, int (*p)[], int (*q)[2])
{
  return condition ? p : q;
}
#endif

#ifdef TEST14
/* Valid case: typedefs preserve the compatible function-pointer type. */
typedef int function_type (int);
typedef function_type *pointer_type;
pointer_type test (int condition, pointer_type p, pointer_type q)
{
  return condition ? p : q;
}
#endif

#ifdef TEST15
/* Valid case: nullptr is allowed with an object pointer; C23 mode enables its spelling. */
#ifndef SKIP_C23
int *test (int condition, int *p)
{
  return condition ? nullptr : p;
}
#endif
#endif

#ifdef TEST16
/* Valid case: compatible function designators decay to function pointers. */
typedef int function_type (int);
typedef function_type *pointer_type;
int first (int);
int second (int);
pointer_type test (int condition)
{
  return condition ? first : second;
}
#endif

#ifdef TEST17
/* Valid case: immediate _Optional qualification is combined without rejecting compatible targets. */
#ifdef __SDCC
_Optional int *test (int condition, int *p, _Optional int *q)
{
  return condition ? p : q;
}
#endif
#endif

#ifdef TEST18
/* Valid case: MCS51 object address spaces retain the existing generic-pointer combination. */
#ifdef __SDCC_mcs51
__data int data_object;
__xdata int xdata_object;
int *test (int condition)
{
  return condition ? &data_object : &xdata_object;
}
#else
typedef int address_space_test_not_applicable;
#endif
#endif

#ifdef TEST19
/* Valid case: a void-pointer null pointer constant may accompany a function pointer. */
typedef int function_type (int);
typedef function_type *pointer_type;
pointer_type test (int condition, pointer_type p)
{
  return condition ? (void *)0 : p;
}
#endif

#ifdef TEST20
/* Incompatible function parameter types must not be accepted as castable pointers. */
typedef int int_function (int);
typedef int char_function (char);
int test (int condition, int_function *p, char_function *q)
{
  return (condition ? p : q) != 0; /* WARNING */
}
#endif

#ifdef TEST21
/* Valid case: both operands have void type */
void first (void);
void second (void);

void test (int condition)
{
  condition ? first () : second ();
}
#endif

#ifdef TEST22
/* Either both operands must have void type or neither */
void action (void);

void test (int condition)
{
  condition ? 1 : action (); /* ERROR(SDCC) */ /* WARNING(GCC) */
}
#endif

#ifdef TEST23
/* Valid case: both operands have arithmetic type */
float test (int condition, int i, float d)
{
  return condition ? i : d;
}
#endif

#ifdef TEST24
#ifndef SKIP_C23
/* Valid case: both operands have nullptr_t type */
typeof_unqual(nullptr) test (int condition)
{
  return condition ? nullptr : nullptr;
}
#endif
#endif

#ifdef TEST25
/* Valid case: both operands are a void-pointer null pointer constant */
void *test (int condition)
{
  return condition ? (void *)0 : (void *)0;
}
#endif

#ifdef TEST26
/* Valid case: the operands are different null pointer constants */
void *test (int condition)
{
  return condition ? (void *)0 : 0;
}
#endif

#ifdef TEST27
/* Valid case: the operands are different null pointer constants */
void *test (int condition)
{
  return condition ? 0 : (void *)0;
}
#endif
