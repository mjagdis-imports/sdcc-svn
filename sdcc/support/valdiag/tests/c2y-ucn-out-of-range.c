/* Compile each selected case separately. */
int audit_nonempty;

#ifdef TEST1
/* A universal character cannot exceed code point 10FFFF. */
int \U00110000 = 0; /* ERROR */ /* WARNING(SDCC) */
#endif
