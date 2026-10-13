/* Explicitly typed auto declarations use ordinary initialisation. */
#pragma std_c23

/* auto int with an int constant must be accepted and declare an int. */
#ifdef TEST1
int f(void)
{
  auto int value = 1;
  _Static_assert(_Generic(&value, int *: 1, default: 0), "int object");
  return value;
}
#endif

/* auto float with an int expression must still declare a float. */
#ifdef TEST2
float f(void)
{
  auto float value = 2;
  _Static_assert(_Generic(&value, float *: 1, default: 0), "float object");
  return value;
}
#endif

/* auto unsigned char must declare unsigned char, even with an int expression. */
#ifdef TEST3
int f(void)
{
  auto unsigned char value = 200;
  _Static_assert(_Generic(&value, unsigned char *: 1, default: 0), "unsigned char object");
  return value;
}
#endif

/* A typedef supplies an explicit type compatible with its aliased type. */
#ifdef TEST4
typedef int I;
int f(void)
{
  auto I value = 1;
  _Static_assert(_Generic(&value, I *: 1, default: 0), "I object");
  return value;
}
#endif

/* auto int with a float expression must still declare an int. */
#ifdef TEST5
int f(void)
{
  auto int value = 2.5f;
  _Static_assert(_Generic(&value, int *: 1, default: 0), "int object");
  return value;
}
#endif

/* Unsigned alone specifies unsigned int. */
#ifdef TEST6
unsigned f(void)
{
  auto unsigned value = 2.5f;
  _Static_assert(_Generic(&value, unsigned int *: 1, default: 0), "unsigned int object");
  return value;
}
#endif

/* Signed alone specifies signed int. */
#ifdef TEST7
int f(void)
{
  auto signed value = 2.5f;
  _Static_assert(_Generic(&value, signed int *: 1, default: 0), "signed int object");
  return value;
}
#endif

/* Short alone specifies short int. */
#ifdef TEST8
short f(void)
{
  auto short value = 2.5f;
  _Static_assert(_Generic(&value, short int *: 1, default: 0), "short int object");
  return value;
}
#endif

/* Long alone specifies long int. */
#ifdef TEST9
long f(void)
{
  auto long value = 2.5f;
  _Static_assert(_Generic(&value, long int *: 1, default: 0), "long int object");
  return value;
}
#endif

/* Long long without int still specifies long long int. */
#ifdef TEST10
long long f(void)
{
  auto long long value = 2.5f;
  _Static_assert(_Generic(&value, long long int *: 1, default: 0), "long long int object");
  return value;
}
#endif

/* An explicitly typed enum object must have a type compatible with that enum. */
#ifdef TEST11
enum E { first = 1 };
int f(void)
{
  auto enum E value = first;
  _Static_assert(_Generic(&value, enum E *: 1, default: 0), "enum E object");
  return value;
}
#endif

/* An explicitly typed structure object must have the declared structure type. */
#ifdef TEST12
struct S { int member; };
int f(struct S source)
{
  auto struct S value = source;
  _Static_assert(_Generic(&value, struct S *: 1, default: 0), "struct S object");
  return value.member;
}
#endif

/* An explicit pointer declarator must declare a pointer to int. */
#ifdef TEST13
int f(int *source)
{
  auto int *value = source;
  _Static_assert(_Generic(&value, int **: 1, default: 0), "int * object");
  return *value;
}
#endif

/* Braced initialisation of auto int must still declare an int. */
#ifdef TEST14
int f(void)
{
  auto int value = {1};
  _Static_assert(_Generic(&value, int *: 1, default: 0), "int object");
  return value;
}
#endif

/* An uninitialised auto int declaration must still declare an int. */
#ifdef TEST15
int f(void)
{
  auto int value;
  _Static_assert(_Generic(&value, int *: 1, default: 0), "int object");
  value = 1;
  return value;
}
#endif

/* Without a type specifier, auto infers int from an int constant. */
#ifdef TEST16
int f(void)
{
  auto value = 1;
  _Static_assert(_Generic(&value, int *: 1, default: 0), "int object");
  return value;
}
#endif

/* A const qualifier without a type specifier permits inference of const int. */
#ifdef TEST17
int f(void)
{
  auto const value = 1;
  _Static_assert(_Generic(&value, const int *: 1, default: 0), "const int object");
  return value;
}
#endif

/* An explicit const int type must retain its qualifier. */
#ifdef TEST18
void f(void)
{
  auto const int value = 1;
  _Static_assert(_Generic(&value, const int *: 1, default: 0), "const int object");
  ++value; /* ERROR */
}
#endif

/* Inference from unsigned char must not apply integer promotions. */
#ifdef TEST19
int f(void)
{
  unsigned char source = 200;
  auto value = source;
  _Static_assert(_Generic(&value, unsigned char *: 1, default: 0), "unsigned char object");
  return value;
}
#endif

/* Inference from a float expression must declare a float. */
#ifdef TEST20
float f(void)
{
  auto value = 2.5f;
  _Static_assert(_Generic(&value, float *: 1, default: 0), "float object");
  return value;
}
#endif
