/* Only definitions require complete adjusted parameter types. */
#ifdef TEST1
struct S;
void f(struct S value) /* ERROR */
{
  (void)value;
}
#endif

#ifdef TEST2
union U;
void f(union U value) /* ERROR */
{
  (void)value;
}
#endif

#ifdef TEST3
struct S;
void f(int first, struct S value) /* ERROR */
{
  (void)first;
  (void)sizeof &value;
}
#endif

#ifdef TEST4
struct S;
union U;
void f(struct S first, union U second) /* ERROR */
{
  (void)first;
  (void)second;
}
#endif

#ifdef TEST5
/* Valid case: a declaration can precede completion of the parameter type. */
struct S;
void f(struct S value);
#endif

#ifdef TEST6
/* Valid case: adjustment makes an unknown-length array parameter a pointer. */
void f(int values[])
{
  (void)values;
}
#endif

#ifdef TEST7
/* Valid case: a pointer to an incomplete structure has complete type. */
struct S;
void f(struct S *value)
{
  (void)value;
}
#endif

#ifdef TEST8
struct S
{
  int member;
};
void f(struct S value)
{
  (void)value;
}
#endif

#ifdef TEST9
union U
{
  int member;
};
void f(union U value)
{
  (void)value;
}
#endif

#ifdef TEST10
/* Valid case: completing the tag before the definition completes its type. */
struct S;
void f(struct S value);
struct S
{
  int member;
};
void f(struct S value)
{
  (void)value;
}
#endif

#ifdef TEST11
/* Valid case: function parameters are adjusted to pointers. */
void f(int callback(void))
{
  (void)callback;
}
#endif
