/*
   bug-3381.c - Incorrect compilation starting in r10263
 */

#include <testfwk.h>

__xdata char ADDR, *ptr;

__xdata struct {
    char* a;
    int b;
} foo;

struct {
    char c;
} bar;

void test(void)
{
    int len = ADDR&1;
    ptr = &foo.a[foo.b];
    bar.c++;
    if (foo.b) len -= foo.b;
    foo.b += len;
}

char buf[16];

void testBug(void)
{
	foo.a = buf;

    ADDR = 0x13;
	foo.b = 0x08;
    bar.c = 5;
	test();
	ASSERT(foo.b==1);
	ASSERT(bar.c==6);

    ADDR = 0x10;
	foo.b = 0x0204;
    bar.c = 7;
	test();
	ASSERT(foo.b==0);
	ASSERT(bar.c==8);
}
