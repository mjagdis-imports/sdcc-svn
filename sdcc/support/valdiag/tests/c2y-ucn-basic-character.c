/* Compile each selected case separately. */
int audit_nonempty;

#ifdef TEST1
/* A basic letter cannot be written as a UCN outside a literal. */
int \u0061 = 0; /* ERROR */
#endif
