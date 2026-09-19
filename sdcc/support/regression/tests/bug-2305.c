/*
   bug-2305.c register packing optimized away the assignment even though code generation for left shift cannot deal with sfr result operand.
 */

#include <testfwk.h>

__sfr __at 0xF4 fd_select;

static void foo(unsigned char x)
{
	fd_select = 1 << x;
}

static void bar(unsigned char x)
{
	unsigned char a = 1 << x;
	fd_select = a;
}

void testBug(void)
{
}

