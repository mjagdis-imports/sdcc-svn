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
void g0(void)
{
	char c[1];
	char *p = 0;
	if(i)
		p = c;
	f3(p); /* WARNING */
}

// Array too short after pointer arithmetic
void g1(void)
{
	char c[3];
	f3(c + 2); /* WARNING */
}

void g2(void)
{
	char c[3];
	f3(c + 1);
}
#endif

#ifdef TEST3
// Dereference instead of parameter passing

char c[3];

char g2(void)
{
	char *p = c + 2;
	return *p;
}

char g3(void)
{
	char *p = c + 3;
	return *p; /* WARNING */
}

char g2a(_Bool i)
{
	char *p = c + 2 + i;
	return *p;
}

char g2b(_Bool i)
{
	char *p;
	if(i)
		p = c + 3;
	else
		p = c + 2;
	return *p;
}

char g3a(_Bool i)
{
	char *p = c + 3 + i;
	return *p; /* WARNING */
}

char g3b(_Bool i)
{
	char *p;
	if(i)
		p = c + 4;
	else
		p = c + 3;
	return *p; /* WARNING */
}
#endif

#ifdef TEST4
// Don't warn if the array length is not known.

extern unsigned long array[]; // Unknown length

unsigned long f(void)
{
	return(array[2]);
}

unsigned long g(void)
{
	return(*array);
}

unsigned long array[] = {0, 1, 2}; // Known length here
#endif

#ifdef TEST5
// Object accessed as array of char.
long l;

char f(void)
{
	char *p = &l;
	return(p[sizeof(long) - 1]);
}

char g(void)
{
	char *p = &l;
	return(p[sizeof(long)]); /* WARNING */
}
#endif

// Check that parameters only reference known other parametrs (C99/C23 syntax)
#ifdef TEST6a
#pragma disable_warning 85
void f(int i, char a[j]); /* ERROR */

void f(int i, char a[j])  /* ERROR */
{
}

void g(char a[i], int i)  /* ERROR */
{
}

void g2(int i, char a[i]);

void g2(int i, char a[i])
{
}
#endif
#ifdef TEST6b
#pragma disable_warning 85
void f(int i, char a[static j]); /* ERROR */

void f(int i, char a[static j])  /* ERROR */
{
}

void g(char a[static i], int i)  /* ERROR */
{
}

void g2(int i, char a[static i]);

void g2(int i, char a[static i])
{
}
#endif

// Check diagnostics for passing null pointers to standard library functions, where not allowed.
#ifdef TEST7a
#include <string.h>
void f(void)
{
	char c[] = "";
	char d[] = "";

	strcpy(c, d);
	strcpy(c, NULL);    /* WARNING */
	strcpy(NULL, d);    /* WARNING */
	strcpy(NULL, NULL); /* WARNING */

	strdup(c);
	strdup(NULL);       /* WARNING */

	strcmp(c, d);
	strcmp(c, NULL);    /* WARNING */
	strcmp(NULL, d);    /* WARNING */
	strcmp(NULL, NULL); /* WARNING */

	strchr(c, 'd');
	strchr(NULL, 'd');  /* WARNING */

	strstr(c, d);
	strstr(c, NULL);    /* WARNING */
	strstr(NULL, d);    /* WARNING */
	strstr(NULL, NULL); /* WARNING */

	strlen(c);
	strlen(NULL);       /* WARNING */
}
#endif

#ifdef TEST7b
#include <stdlib.h>
#include <wchar.h>

void f(void)
{
	atoi(NULL);               /* WARNING */
	strtoull("", NULL, 10);
	strtoull(NULL, NULL, 10); /* WARNING */
	wcstol(NULL, NULL, 10);   /* WARNING */
	wcscmp(L"", L"");
	wcscmp(NULL, L"");        /* WARNING */
	wcscmp(L"", NULL);        /* WARNING */
	wcscmp(NULL, NULL);       /* WARNING */
}
#endif

// Check well-formedness of parameter forward declarations
#ifdef TEST8
#pragma std_sdcc23
// Forward declared parameter not named
void f0(int; int j);       /* ERROR */

// Forward declared parameter missing in parameter list
void f1(int i; char a[i]); /* ERROR */

void f2(int i; char a[i], int i);

void f3(int i; int i);

// Not integer type
void f4(float i; float i); /* ERROR */

// Not integer type
void f5(int *i; int *i);   /* ERROR */

void f6(_BitInt(8) i; char a[i], _BitInt(8) i);

// Types don't match
void f7(int i; long i);    /* ERROR */
#endif

