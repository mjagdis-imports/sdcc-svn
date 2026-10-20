/* Bug #3016: the original malformed array bound crashed validateLink. */
#ifdef TEST1C99
char arr[f(d) ? 0 : 0]; /* ERROR */ /* WARNING */
#endif
