/* N3886 6.5.4.5p1: alignof requires a complete object type or an array thereof. */

#ifdef TEST1
#pragma std_c2y
struct incomplete;
enum { alignment = _Alignof(struct incomplete) }; /* ERROR */
#endif

#ifdef TEST2
#pragma std_c2y
enum { alignment = _Alignof(int[][]) }; /* ERROR */
#endif

#ifdef TEST3
#pragma std_c2y
enum { alignment = _Alignof(int[2][]) }; /* ERROR */
#endif

#ifdef TEST4
#pragma std_c2y
struct incomplete;
enum { alignment = _Alignof(struct incomplete[]) }; /* ERROR */
#endif

#ifdef TEST5
#pragma std_c2y
enum { alignment = _Alignof(void) }; /* ERROR */
#endif

#ifdef TEST6
#pragma std_c2y
typedef int Function(void);
enum { alignment = _Alignof(Function) }; /* ERROR */
#endif

#ifdef TEST7
#pragma std_c2y
_Static_assert(_Alignof(int[]) == _Alignof(int), "array alignment");
#endif

#ifdef TEST8
#pragma std_c2y
_Static_assert(_Alignof(int[][3]) == _Alignof(int[3]), "array alignment");
#endif

#ifdef TEST9
#pragma std_c2y
_Static_assert(_Alignof(int[2][3]) == _Alignof(int), "array alignment");
#endif

#ifdef TEST10
#pragma std_c2y
struct incomplete;
_Static_assert(_Alignof(struct incomplete *) == 1, "pointer alignment");
#endif

#ifdef TEST11
#pragma std_c11
enum { alignment = _Alignof(int[]) }; /* WARNING */
#endif

#ifdef TEST12
#pragma std_c23
enum { alignment = _Alignof(int[]) }; /* WARNING */
#endif

#ifdef TEST13
#pragma std_c23
_Static_assert(_Alignof(int[2][3]) == _Alignof(int), "array alignment");
#endif

#ifdef TEST14
#pragma std_c2y
/* A zero-sized definition is complete; size is not a completeness test. */
struct zero { unsigned : 0; };
_Static_assert(_Alignof(struct zero) == 1, "complete zero-sized structure");
#endif

#ifdef TEST15
#pragma std_c11
enum { alignment = _Alignof(int[][3][]) }; /* ERROR */
#endif

