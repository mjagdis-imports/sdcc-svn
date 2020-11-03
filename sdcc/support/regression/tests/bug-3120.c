/*
   bug-3120.c
   A reliable reproducer for bug in array initalization resulting in
   sporadic failure for bug2942247.c.
 */

#include <testfwk.h>

#include <stddef.h> // For size_t


// By using our own declaration of memset instead of including string.h we
// avoid optimizations based on compiler knowledge about memset.
void *memset(void *s, int c, size_t n);

_Bool bar2(void)
{
	_Bool foo[2] = {1, 2};
	return foo[1];
}

_Bool bar6(void)
{
	_Bool foo[6] = {1, 2};
	return foo[1];
}

_Bool bar2wrapper(void)
{
	return(bar2());
}

_Bool bar6wrapper(void)
{
	return(bar6());
}

void zerostack(void)
{
	char array[10];
	memset(array, 0, 10);
}

void testBug(void)
{
	zerostack();
	ASSERT(bar2wrapper());
	zerostack();
	ASSERT(bar6wrapper());
}

