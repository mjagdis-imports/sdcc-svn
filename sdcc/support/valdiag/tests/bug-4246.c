/* Incomplete-array typedefs do not declare tentative object definitions. */
#ifdef __SDCC
#pragma std_c11
#endif

#ifdef TEST1
typedef int Tail[];
#endif

#ifdef TEST2
typedef int Tail[];
typedef Tail Alias;
#endif

#ifdef TEST3
typedef const int Tail[];
#endif

#ifdef TEST4
typedef int Tail[];
struct S { int n; Tail data; };
#endif

#ifdef TEST5
typedef int Tail[];
Tail object = {1, 2, 3};
#endif

#ifdef TEST6
typedef int Tail[];
Tail first = {1, 2};
Tail second = {3, 4, 5};
#endif

#ifdef TEST7
typedef int Tail[];
extern Tail object;
#endif

#ifdef TEST8
typedef int Tail[];
Tail *pointer;
#endif

#ifdef TEST9
typedef int Tail[];
struct S { Tail *pointer; };
#endif

#ifdef TEST10
typedef int Tail[];
Tail object; /* WARNING */
#endif

#ifdef TEST11
int object[]; /* WARNING */
#endif

#ifdef TEST12
static int object[]; /* WARNING */ /* ERROR(GCC) */
#endif

#ifdef TEST13
extern int object[];
#endif

#ifdef TEST14
int object[2];
#endif

#ifdef TEST15
typedef int Tail[];
Tail object;
Tail object = {1, 2, 3};
#endif

#ifdef TEST16
void test(void)
{
  typedef int Tail[];
  Tail object = {1, 2};
  (void)object;
}
#endif

#ifdef TEST17
typedef int Tail[];
Tail first; /* WARNING */
Tail second; /* WARNING */
#endif

#ifdef TEST18
typedef int Tail[];
extern Tail object;
Tail object; /* WARNING */
#endif

#ifdef TEST19
typedef int Tail[2];
#endif

#ifdef TEST20
typedef int Tail[];
struct First { int n; Tail data; };
struct Second { int n; Tail data; };
#endif

