/* N3886 constraint cases, each compiled separately. */
int audit_nonempty;


#ifdef TEST2
/* The file-scope combination does not infer a type. */
static auto int x = 1; /* ERROR */
#endif
