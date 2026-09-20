/*
   bug-2855.c

   Missing non-inline definition of aligned_alloc
*/

#include <testfwk.h>

#include <stdlib.h>

#if __STDC_VERSION__ >= 201112L
#if !defined(__APPLE__)
void *(*volatile f)(size_t, size_t) __reentrant = &aligned_alloc;
#endif
#endif

void testBug(void)
{
#if __STDC_VERSION__ >= 201112L
#if !defined(__APPLE__)
	int *buffer = (*f)(_Alignof(int), sizeof(int) * 2);
	buffer[0] = 23;
	buffer[1] = 42;
	ASSERT (buffer[0] == 23);
	ASSERT (buffer[1] == 42);
	free (buffer);
#endif
#endif
}

