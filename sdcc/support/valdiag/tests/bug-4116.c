/* Defining declarations of the same struct or union must not be nested. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

/* Keep skipped cases nonempty for host compilers in pedantic mode. */
void unrelated(void);

/* Put invalid tags and their braces on one line to align diagnostic locations. */
#ifdef TEST1
/* A nested definition must not complete the enclosing structure. */
struct S
{
  struct S { /* ERROR */
    int x;
  } a;
};
#endif

#ifdef TEST2
/* The same restriction applies to unions. */
union U
{
  union U { /* ERROR */
    int x;
  } a;
};
#endif

#ifdef TEST3
/* A pointer member does not make a nested structure definition valid. */
struct S
{
  struct S { /* ERROR */
    int x;
  } *p;
};
#endif

#ifdef TEST4
/* A pointer member does not make a nested union definition valid. */
union U
{
  union U { /* ERROR */
    int x;
  } *p;
};
#endif

#ifdef TEST5
/* Completing a forward-declared structure must mark it as being defined. */
struct S;
struct S
{
  struct S { /* ERROR */
    int x;
  } a;
};
#endif

#ifdef TEST6
/* Completing a forward-declared union must mark it as being defined. */
union U;
union U
{
  union U { /* ERROR */
    int x;
  } a;
};
#endif

#ifdef TEST7
/* An intervening member type does not end the outer structure definition. */
struct S
{
  struct T
  {
    struct S { /* ERROR */
      int x;
    } a;
  } b;
};
#endif

#ifdef TEST8
/* An intervening member type does not end the outer union definition. */
union U
{
  union V
  {
    union U { /* ERROR */
      int x;
    } a;
  } b;
};
#endif

#ifdef TEST9
/* Structure and union tags share a namespace. */
struct S
{
  union S { /* ERROR */
    int x;
  } a;
}; /* ERROR(GCC) */
#endif

#ifdef TEST10
/* Reusing a union tag for a nested structure is invalid too. */
union U
{
  struct U { /* ERROR */
    int x;
  } a;
}; /* ERROR(GCC) */
#endif

#ifdef TEST11
#ifndef SKIP_C23
/* A nested definition is invalid even inside a C23 redefinition.
   The changed members also make the enclosing redefinition incompatible. */
struct S /* ERROR(SDCC) */ /* IGNORE(GCC) */
{
  int x;
};
struct S /* IGNORE(GCC) */
{
  struct S { /* ERROR(SDCC) */ /* IGNORE(GCC) */
    int x;
  } *p;
}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST12
#ifndef SKIP_C23
/* Exercise the corresponding C23 redefinition path for a union. */
union U /* ERROR(SDCC) */ /* IGNORE(GCC) */
{
  int x;
};
union U /* IGNORE(GCC) */
{
  union U { /* ERROR(SDCC) */ /* IGNORE(GCC) */
    int x;
  } *p;
}; /* ERROR(SDCC) */ /* IGNORE(GCC) */
#endif
#endif

#ifdef TEST13
/* A reference to the type being defined is not a nested definition. */
struct S
{
  struct S *next;
};
union U
{
  union U *next;
};
_Static_assert(_Generic(((struct S *)0)->next, struct S *: 1, default: 0),
               "self-pointer refers to the enclosing structure");
_Static_assert(_Generic(((union U *)0)->next, union U *: 1, default: 0),
               "self-pointer refers to the enclosing union");
#endif

#ifdef TEST14
/* Different nested tags remain valid and retain their member types. */
struct S
{
  struct T
  {
    int x;
  } a;
};
union U
{
  union V
  {
    int x;
    char y;
  } a;
  int b;
};
_Static_assert(sizeof(((struct S *)0)->a) == sizeof(int),
               "nested structure retains its layout");
_Static_assert(_Generic(((struct S *)0)->a.x, int: 1, default: 0),
               "nested structure member retains its type");
_Static_assert(_Generic(((union U *)0)->a.y, char: 1, default: 0),
               "nested union member retains its type");
#endif

#ifdef TEST15
/* A block-scope definition shadows the outer tag without replacing it. */
struct S
{
  int x;
};
void f(void)
{
  {
    struct S
    {
      char y;
    } inner;
    _Static_assert(_Generic(inner.y, char: 1, default: 0),
                   "inner tag has its own members");
  }
  struct S outer;
  _Static_assert(_Generic(outer.x, int: 1, default: 0),
                 "outer tag is still visible after the block");
}
#endif

#ifdef TEST16
#ifndef SKIP_C23
/* C23 permits completed definitions to repeat, including nested member types. */
struct S
{
  struct T
  {
    int x;
  } a;
  struct S *next;
};
struct S
{
  struct T
  {
    int x;
  } a;
  struct S *next;
};
struct S
{
  struct T a;
  struct S *next;
};
_Static_assert(_Generic(((struct S *)0)->a.x, int: 1, default: 0),
               "repeated structure definition retains its member type");
_Static_assert(_Generic(((struct S *)0)->next, struct S *: 1, default: 0),
               "repeated structure definition retains its self-pointer");
#endif
#endif

#ifdef TEST17
#ifndef SKIP_C23
/* Repeated union definitions must also finish their nested member types. */
union U
{
  union V
  {
    int x;
    char y;
  } a;
  int b;
};
union U
{
  union V
  {
    int x;
    char y;
  } a;
  int b;
};
union U
{
  union V a;
  int b;
};
_Static_assert(_Generic(((union U *)0)->a.y, char: 1, default: 0),
               "repeated union definition retains its member type");
#endif
#endif

#ifdef TEST18
/* Anonymous nested types must not be mistaken for a repeated tag. */
struct S
{
  struct
  {
    int x;
  } a;
};
union U
{
  union
  {
    int x;
  } a;
};
#endif

#ifdef TEST19
/* A forward declaration can still be completed without nested definitions. */
struct S;
struct S
{
  int x;
};
union U;
union U
{
  int x;
};
#endif
