/* Flexible array members require a structure with another named member. */
#ifdef __SDCC
#pragma std_c11
#endif

void unrelated (void);

#ifdef TEST1
union U { int n; int data[]; }; /* ERROR */
#endif

#ifdef TEST2
union U { int data[]; }; /* ERROR */
#endif

#ifdef TEST3
typedef int Tail[];
union U { int n; Tail data; }; /* ERROR */
#endif

#ifdef TEST4
struct S { unsigned : 1; int data[]; }; /* ERROR */
#endif

#ifdef TEST5
struct S { unsigned : 0; int data[]; }; /* ERROR */
#endif

#ifdef TEST6
struct S { unsigned : 1; unsigned : 2; int data[]; }; /* ERROR */
#endif

#ifdef TEST7
struct S { unsigned : 1; unsigned : 0; int data[]; }; /* ERROR */
#endif

#ifdef TEST8
typedef unsigned char Tail[];
struct S { unsigned : 1; Tail data; }; /* ERROR */
#endif

#ifdef TEST9
struct S { int data[]; }; /* ERROR */
#endif

#ifdef TEST10
struct S { int n; int data[]; int after; }; /* ERROR */
#endif

#ifdef TEST11
struct S { unsigned char n; unsigned char data[]; };
#endif

#ifdef TEST12
struct S { unsigned n : 1; int data[]; };
#endif

#ifdef TEST13
struct S { unsigned : 1; unsigned n : 1; int data[]; };
#endif

#ifdef TEST14
struct S { int n; unsigned : 0; int data[]; };
#endif

#ifdef TEST15
struct S { struct { int n; }; int data[]; };
#endif

#ifdef TEST16
struct S { union { int n; unsigned char c; }; int data[]; };
#endif

#ifdef TEST17
struct S { struct { struct { int n; }; }; int data[]; };
#endif

#ifdef TEST18
struct S { unsigned char n; unsigned char data[]; };
union U { struct S packet; int n; };
#endif

#ifdef TEST19
typedef int Tail[1];
union U { int n; Tail data; };
#endif

#ifdef TEST20
typedef int Tail[];
struct S { int n; Tail data; };
#endif

#ifdef TEST21
union U { int n; int *data; };
#endif

#ifdef TEST22
#ifdef __SDCC
/* Preserve the existing zero-length array extension in unions. */
union U { int n; int data[0]; };
#endif
#endif
