int audit_nonempty;

/* A control character cannot occur in an identifier as a UCN. */
int audit_nonempty;

#ifdef TEST1
int \u0001name; /* ERROR */
#endif
