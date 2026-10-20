/* Bug #2660: no spurious pointer-type warning for this static initialiser. */
#ifdef TEST1C99
static int cmp_eq(long arg1, long arg2)
{
  return arg1 != arg2;
}

struct op
{
  const char *op_name;
  void *op_func;
};

const struct op string_binop[] = {
  {"=", (void *)cmp_eq}, /* WARNING (GCC || SDCC_mcs51 || SDCC_ds390) */
};
#endif
