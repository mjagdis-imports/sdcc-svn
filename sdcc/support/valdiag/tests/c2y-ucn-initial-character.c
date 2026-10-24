/* Compile each selected case separately. */
int audit_nonempty;

#ifdef TEST1
/* A combining character cannot start an identifier. */
int \u0300 = 0; /* ERROR */
#endif
