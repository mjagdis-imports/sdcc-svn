/* Regression coverage for bug #4117. */
#ifdef __SDCC
#pragma std_c23
#endif

#ifdef TEST1
/* pointer_enum */
typedef int *P;
enum E : P
{
  e
}; /* ERROR */
#endif

#ifdef TEST2
/* array_enum */
typedef int A[2];
enum E : A
{
  e
}; /* ERROR */
#endif

#ifdef TEST3
/* function_enum */
typedef int F (void);
enum E : F
{
  e
}; /* ERROR */
#endif

#ifdef TEST4
/* pointer_forward */
typedef int *P;
enum E : P; /* ERROR */
#endif

#ifdef TEST5
/* array_forward */
typedef int A[2];
enum E : A; /* ERROR */
#endif

#ifdef TEST6
/* function_forward */
typedef int F (void);
enum E : F; /* ERROR */
#endif

#ifdef TEST7
/* typedef_chain */
typedef int *P;
typedef P Q;
enum E : Q
{
  e
}; /* ERROR */
#endif

#ifdef TEST8
/* enum_underlying */
typedef enum A : int
{
  a
} EA;
enum E : EA
{
  e
}; /* ERROR */
#endif

#ifdef TEST9
/* float_underlying */
typedef float F;
enum E : F
{
  e
}; /* ERROR */
#endif

#ifdef TEST10
/* void_underlying */
typedef void V;
enum E : V
{
  e
}; /* ERROR */
#endif

#ifdef TEST11
/* integer_typedef */
typedef int I;
typedef I J;
enum E : J
{
  low = -2, high = 3
};
#endif

#ifdef TEST12
/* unsigned_typedef */
typedef unsigned long U;
enum E : U
{
  e = 65536
};
#endif

#ifdef TEST13
/* integer_forward */
typedef int I;
enum E : I;
enum E : I
{
  e
};
#endif

#ifdef TEST14
/* char_typedef */
typedef unsigned char C;
enum E : C
{
  e = 255
};
#endif

#ifdef TEST15
/* bool_typedef */
typedef _Bool B;
enum E : B
{
  e = 1
};
#endif

#ifdef TEST16
/* _BitInt typedef */
#pragma std_c23
typedef _BitInt(7) B;
enum E : B
{
  e = 50 /* WARNING */
};
#endif

#ifdef TEST17
#pragma std_c2y
/* _BitInt typedef */
typedef _BitInt(7) B;
enum E : B
{
  e = 50
};
#endif

