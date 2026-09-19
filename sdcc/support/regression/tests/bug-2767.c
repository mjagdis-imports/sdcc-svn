/*
   bug1-2195.c

   Assertion failure in z80 code generation.
*/

#include <testfwk.h>

void f(void)
{
	((void (*)(int)) 0)(0);
}

void testBug(void)
{
}

