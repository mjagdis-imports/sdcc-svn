#ifdef TEST1
int f3(char c[static 2]);

// Passing null pointer
void g0(void)
{
	f3(0); /* TODO */
}

// Passing array that is too short
void g1(void)
{
	char a[1];
	f3(a); /* TODO */
}

// Passing suitable array
void g2(void)
{
	char a[2];
	f3(a);
}
#endif

