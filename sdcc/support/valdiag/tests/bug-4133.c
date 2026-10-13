/* Repeated definitions require corresponding members to have the same type. */
#ifdef __SDCC
#pragma std_c23
#endif

#ifdef TEST1
struct S /* ERROR */
{
  int (*member)[];
};
struct S
{
  int (*member)[2];
}; /* ERROR */
#endif

#ifdef TEST2
struct S /* ERROR */
{
  int (*member)[2];
};
struct S
{
  int (*member)[];
}; /* ERROR */
#endif

#ifdef TEST3
struct S /* ERROR */
{
  int member;
};
struct S
{
  const int member;
}; /* ERROR */
#endif

#ifdef TEST4
struct S /* ERROR */
{
  int member;
};
struct S
{
  volatile int member;
}; /* ERROR */
#endif

#ifdef TEST5
union U /* ERROR */
{
  int member;
};
union U
{
  const int member;
}; /* ERROR */
#endif

#ifdef TEST6
struct S /* ERROR */
{
  int *member;
};
struct S
{
  const int *member;
}; /* ERROR */
#endif

#ifdef TEST7
struct S /* ERROR */
{
  int *member;
};
struct S
{
  int *const member;
}; /* ERROR */
#endif

#ifdef TEST8
/* Valid case: identical const-qualified members. */
struct S
{
  const int member;
};
struct S
{
  const int member;
};
#endif

#ifdef TEST9
/* Valid case: a typedef does not create a different member type. */
typedef int Integer;
struct S
{
  int member;
};
struct S
{
  Integer member;
};
#endif

#ifdef TEST10
/* Valid case: identical pointers to arrays of unknown bound. */
struct S
{
  int (*member)[];
};
struct S
{
  int (*member)[];
};
#endif

#ifdef TEST11
/* Valid case: function parameter arrays are adjusted to pointers. */
struct S
{
  int (*member)(int []);
};
struct S
{
  int (*member)(int *);
};
#endif

#ifdef TEST12
/* Valid case: outer parameter qualifiers do not change a function type. */
struct S
{
  int (*member)(int);
};
struct S
{
  int (*member)(const int);
};
#endif

#ifdef TEST13
/* Bit-precise member types must retain their qualifiers too. */
struct S /* ERROR */
{
  _BitInt(8) member;
};
struct S
{
  const _BitInt(8) member;
}; /* ERROR */
#endif

#ifdef TEST14
struct S /* ERROR */
{
  _BitInt(8) *member;
};
struct S
{
  const _BitInt(8) *member;
}; /* ERROR */
#endif
