/*
  bug-2548.c
*/

#include <testfwk.h>

static int cmp_eq (long arg1, long arg2)
{
    return arg1 != arg2;
}
struct op {
    const char *op_name;
    void (*op_func)(void);
};

/* In initializations, SDCC did not allow some function pointer casts allowed by the standard. */
#ifdef __SDCC
const struct op string_binop1[] = {
    {"=", (void (*)(void*))cmp_eq},
};

const struct op string_binop2[] = {
    {"=", (void (*)(void*))&cmp_eq},
};
#endif

void testBug(void)
{
#ifdef __SDCC
#if !defined(__SDCC_mcs51) && !defined(__SDCC_ds390) /* mcs51, hc08, s08 and pdk14 have restrictions on function pointers wrt. reentrancy */
	ASSERT(((int (*)(long, long))(string_binop1[0].op_func))(1, 1) == 0);
	ASSERT(((int (*)(long, long))(string_binop1[0].op_func))(1, 2) == 1);
	ASSERT(((int (*)(long, long))(string_binop2[0].op_func))(1, 1) == 0);
	ASSERT(((int (*)(long, long))(string_binop2[0].op_func))(1, 2) == 1);
#endif
#endif
}

