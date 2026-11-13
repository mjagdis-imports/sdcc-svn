/* A constexpr bool initialiser must represent its original value exactly. */
#ifdef __SDCC
#pragma std_c23
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define SKIP_C23
#endif

void unrelated (void);

#ifndef SKIP_C23

#ifdef TEST1
constexpr _Bool object = 2; /* ERROR */
#endif

#ifdef TEST2
constexpr _Bool object = -1; /* ERROR */
#endif

#ifdef TEST3
constexpr _Bool object = 255u; /* ERROR */
#endif

#ifdef TEST4
constexpr _Bool object = 0x100000000ULL; /* ERROR */
#endif

#ifdef TEST5
constexpr _Bool object = 0xffffffffffffffffULL; /* ERROR */
#endif

#ifdef TEST6
constexpr _Bool object = (-9223372036854775807LL - 1); /* ERROR */
#endif

#ifdef TEST7
constexpr _Bool values[] = {0, 2}; /* ERROR */
#endif

#ifdef TEST8
struct S { int count; _Bool flag; };
constexpr struct S object = {1, 2}; /* ERROR */
#endif

#ifdef TEST9
struct S { _Bool flags[2]; };
constexpr struct S object = {{0, -1}}; /* ERROR */
#endif

#ifdef TEST10
union U { _Bool flag; int number; };
constexpr union U object = {.flag = 2}; /* ERROR */
#endif

#ifdef TEST11
struct S { _Bool flag : 1; };
constexpr struct S object = {2}; /* ERROR */
#endif

#ifdef TEST12
constexpr int source = 2;
constexpr _Bool object = source; /* ERROR */
#endif

#ifdef TEST13
int test(void)
{
  constexpr _Bool object = 2; /* ERROR */
  return object;
}
#endif

#ifdef TEST14
int test(void)
{
  constexpr _Bool values[2] = {0, -1}; /* ERROR */
  return values[1];
}
#endif

#ifdef TEST15
struct S { _Bool flag; };
int test(void)
{
  constexpr struct S object = {2}; /* ERROR */
  return object.flag;
}
#endif

#ifdef TEST16
struct S { _Bool flag : 1; };
int test(void)
{
  constexpr struct S object = {2}; /* ERROR */
  return object.flag;
}
#endif

#ifdef TEST17
constexpr _Bool object = 1 + 1; /* ERROR */
#endif

#ifdef TEST18
constexpr _Bool object = (unsigned char)2; /* ERROR */
#endif

#ifdef TEST19
constexpr _Bool object = 0;
#endif

#ifdef TEST20
constexpr _Bool object = 1;
#endif

#ifdef TEST21
constexpr _Bool object = (_Bool)2;
#endif

#ifdef TEST22
constexpr _Bool object = (_Bool)-1;
#endif

#ifdef TEST23
constexpr _Bool values[] = {0, 1, (_Bool)2};
#endif

#ifdef TEST24
struct S { _Bool flag; int number; };
constexpr struct S object = {1, 2};
#endif

#ifdef TEST25
struct S { _Bool flag : 1; };
constexpr struct S object = {(_Bool)2};
#endif

#ifdef TEST26
union U { _Bool flag; int number; };
constexpr union U object = {.number = 2};
#endif

#ifdef TEST27
_Bool object = 2;
#endif

#ifdef TEST28
const _Bool object = 2;
#endif

#ifdef TEST29
int test(void)
{
  _Bool object = 2;
  return object;
}
#endif

#ifdef TEST30
int test(void)
{
  constexpr _Bool object = (_Bool)2;
  return object;
}
#endif

#ifdef TEST31
struct S { _Bool flags[3]; };
constexpr struct S object = {{[2] = 1}};
#endif

#ifdef TEST32
constexpr unsigned long long source = 0x100000000ULL;
constexpr _Bool object = source; /* ERROR */
#endif

#endif /* !SKIP_C23 */
