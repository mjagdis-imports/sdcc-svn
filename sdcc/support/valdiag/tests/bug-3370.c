/* Bug #3370: excess elements after a function definition caused an ICE. */
#ifdef TEST1C99
void f(void)
{
}

const int i[] = {{1, 2}, {3, 4}}; /* WARNING */
#endif
