/* bug-4056.c

   Some cases of missing diagnostics on pointer arithmetic on incompletet types.
 */

#ifdef TEST1
void *p;

void inc(void) // Not affected by bug
{
	p++; /* WARNING */
}

void dec(void) // Not affected by bug
{
	p--; /* WARNING */
}


void add(void) // Affected by bug
{
	p += 7; /* WARNING */
}

void sub(void) // Affected by bug
{
	p -= 7; /* WARNING */
}
#endif

#ifdef TEST2
struct s;

struct s *p;

void inc(void) // Not affected by bug
{
	p++; /* ERROR */
}
#endif

#ifdef TEST3
struct s;

struct s *p;

void add(void) // Affected by bug
{
	p += 7; /* ERROR */
}
#endif

#ifdef TEST4
struct s;

struct s *p;

void sub(void) // Affected by bug
{
	p -= 7; /* ERROR */
}
#endif

