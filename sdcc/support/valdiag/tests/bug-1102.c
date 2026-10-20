/* Bug #1102: diagnose an initialiser for an incomplete structure. */
#ifdef TEST1C99
const struct test x = {0}; /* ERROR */
#endif
