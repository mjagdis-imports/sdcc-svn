int audit_nonempty;

#ifdef TEST3
/* A surrogate code point cannot be named by a universal character name. */
const char *surrogate = "\uD800"; /* WARNING(SDCC) */ /* ERROR(GCC) */
#endif
