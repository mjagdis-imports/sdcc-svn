/*
   C23 typeof, typeof_unqual
*/

#include <testfwk.h>

#ifdef __SDCC
#pragma std_c23

typeof(7l) l7 = 7;

int typeof_external_function(void);
extern const volatile int typeof_external_object;

static void
gextern(void)
{
	typeof(typeof_external_function()) from_call = 3;
	typeof(typeof_external_object) from_object = 4;
	ASSERT(from_call == 3);
	ASSERT(from_object == 4);
}

void g0(long l)
{
	typeof_unqual(const int) i = l + 1;
	i++;
	ASSERT(sizeof(i) == sizeof(int));
}

void g1(long l)
{
	typeof_unqual(l + 2) i = l + 1;
	i++;
	ASSERT(sizeof(i) == sizeof(long));
}

void g2(char c)
{
	typeof_unqual(c + 2 /* promoted to int */) i = c + 1;
	i++;
	ASSERT(sizeof(i) == sizeof(int));
}
#endif

void
testTypeof(void)
{
#ifdef __SDCC
	g0(0);
	g1(0);
	g2(0);
	gextern();
	ASSERT(sizeof(l7) == sizeof(long));
#endif
}

