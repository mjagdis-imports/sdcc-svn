#include <stddef.h>

#ifdef TEST1
// Basic functionality

int f3(char c[static 2]);

// Passing null pointer
void g0(void)
{
	f3(NULL); /* WARNING */
}

// Passing array that is too short
void g1a(void)
{
	char a[1];
	f3(a); /* WARNING */
}

// Passing address of single object, where array of length more than 1 is needed
void g1b(void)
{
	char a;
	f3(&a); /* WARNING */
}

// Passing suitable array
void g2(void)
{
	char a[2];
	f3(a);
}
#endif

#ifdef TEST2
// Generalized constant propagation analysis information needs to be used

int f3(char c[static 2]);

int i;

// Multiple possible pointer targets, but all are too short
void g0 (void)
{
	char c[1];
	char *p = 0;
	if (i)
		p = c;
	f3 (p); /* WARNING */
}

// Array too short after pointer arithmetic
void g1 (void)
{
	char c[3];
	f3 (c + 2); /* WARNING */
}

void g2 (void)
{
	char c[3];
	f3 (c + 1);
}
#endif

#ifdef TEST3
// Dereference instead of parameter passing

char c[3];

char g2 (void)
{
	char *p = c + 2;
	return *p;
}

char g3 (void)
{
	char *p = c + 3;
	return *p; /* WARNING */
}

char g2a (_Bool i)
{
	char *p = c + 2 + i;
	return *p;
}

char g2b (_Bool i)
{
	char *p;
	if (i)
		p = c + 3;
	else
		p = c + 2;
	return *p;
}

char g3a (_Bool i)
{
	char *p = c + 3 + i;
	return *p; /* WARNING */
}

char g3b (_Bool i)
{
	char *p;
	if (i)
		p = c + 4;
	else
		p = c + 3;
	return *p; /* WARNING */
}
#endif

