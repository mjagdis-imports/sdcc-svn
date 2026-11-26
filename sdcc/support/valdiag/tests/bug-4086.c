/* N3886 6.5.3.2p2: [] requires a pointer to complete object type
   or an array operand. */
#ifdef TEST1
struct incomplete;
struct incomplete *pointer;
void test (void)
{
  pointer[0]; /* ERROR */
}
#endif

/* A pointer to an array of unknown length also points to an incomplete type. */
#ifdef TEST2
int (*pointer)[];
void test (void)
{
  pointer[0]; /* ERROR */
}
#endif

/* Subscripting void pointers is accepted as an extension, but must warn. */
#ifdef TEST3
void *pointer;
void test (void)
{
  pointer[0]; /* WARNING */
}
#endif

/* The warning also applies in an ISO mode. */
#ifdef TEST4
#pragma std_c23
void *pointer;
void test (void)
{
  pointer[0]; /* WARNING */
}
#endif

/* SDCC extension mode does not suppress the warning. */
#ifdef TEST5
#pragma std_sdcc23
void *pointer;
void test (void)
{
  pointer[0]; /* WARNING */
}
#endif

/* A pointer to a complete object type remains a valid case. */
#ifdef TEST6
int *pointer;
int test (void)
{
  return pointer[0];
}
#endif
