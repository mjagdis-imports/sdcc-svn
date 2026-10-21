/* Bug #3835: both reduced examples from the report's discussion.
   Compile only: the integer addresses are memory-mapped device registers. */
#ifdef TEST1C99
struct S
{
  char arr[3];
};

void f(void)
{
  ((struct S *)0x5678)->arr[0] = 1; /* WARNING (SDCC_mcs51 || SDCC_ds390) */
}

struct outer
{
  struct
  {
    int val;
  } inner;
};

void g(void)
{
  ((struct outer *)0x4000)->inner.val; /* WARNING (GCC || SDCC_mcs51 || SDCC_ds390) */
}
#endif
