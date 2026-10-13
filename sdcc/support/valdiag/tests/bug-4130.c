/* Literal value metadata must not qualify typeof declarations. */
#ifdef __SDCC
#pragma std_c23
#endif

#ifdef TEST1
typeof(nullptr) f(int condition)
{
  return condition ? nullptr : nullptr;
}
#endif

#ifdef TEST2
void f(void)
{
  typeof(nullptr) value = nullptr;
  value = nullptr;
  (void)value;
}
#endif

#ifdef TEST3
void f(void)
{
  typeof(0) value = 0;
  value = 1;
  (void)value;
}
#endif

#ifdef TEST4
void f(void)
{
  typeof(true) value = true;
  value = false;
  (void)value;
}
#endif

#ifdef TEST5
void f(void)
{
  typeof(1 + 2) value = 3;
  value = 4;
  (void)value;
}
#endif

#ifdef TEST6
/* Genuine object qualifiers are retained. */
const int original = 1;
void f(void)
{
  typeof(original) value = 2;
  value = 3; /* ERROR */
  (void)value;
}
#endif

#ifdef TEST7
/* Folding a constexpr object must not remove its const qualification. */
constexpr int original = 1;
void f(void)
{
  typeof(original) value = 2;
  value = 3; /* ERROR */
  (void)value;
}
#endif

#ifdef TEST8
/* typeof_unqual still removes genuine object qualifiers. */
const int original = 1;
void f(void)
{
  typeof_unqual(original) value = 2;
  value = 3;
  (void)value;
}
#endif

#ifdef TEST9
/* A real return-type qualifier must still produce the existing warning. */
const int original = 1;
typeof(original) f(void)
{ /* WARNING(SDCC) */
  return original;
}
#endif

#ifdef TEST10
/* An enumerator is a constant value, not a const-qualified object. */
enum E { first = 1 };
void f(void)
{
  typeof(first) value = first;
  value = 2;
  (void)value;
}
#endif

#ifdef TEST11
/* Removing literal metadata must not remove pointer target qualifiers. */
const int original = 1;
void f(void)
{
  typeof(&original) pointer = &original;
  *pointer = 2; /* ERROR */
}
#endif
