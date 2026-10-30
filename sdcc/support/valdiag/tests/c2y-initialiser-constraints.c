#ifdef __SDCC
#pragma std_c2y
#endif
/* Additional N3886 diagnostic cases. */
int audit_nonempty;

/* N3886 6.7.1p2: a declaration without a declarator must directly
   declare a tag or provide enumerators. */
#ifdef TEST1
int; /* WARNING */
#endif

/* N3886 6.7.11p4: an array of unknown size cannot have an empty initialiser. */
#ifdef TEST2
int array[] = {}; /* ERROR */
#endif

/* Valid tag-only declaration and designated array initialisation. */
#ifdef TEST5
struct complete;
struct complete { int member; };
int array[3] = { [1] = 5 };
#endif
