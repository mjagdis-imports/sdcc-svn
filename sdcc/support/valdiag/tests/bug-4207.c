/* Evaluated calls still prevent constant initialisation. */
int g (void);

#ifdef TEST1
static int a = 1 && g (); /* ERROR */
#endif

#ifdef TEST2
static int a = 0 || g (); /* ERROR */
#endif

#ifdef TEST3
/* A comma expression must not hide an evaluated call. */
static int a = (g (), 0) && 1; /* ERROR */
#endif

#ifdef TEST4
/* Supplying a constant right operand does not suppress the left call. */
static int a = g () && 0; /* ERROR */
#endif

#ifdef TEST5
static int a = g () || 1; /* ERROR */
#endif
