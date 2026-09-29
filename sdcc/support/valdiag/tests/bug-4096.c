/* N3886 6.7.11p4: the entity to be initialized shall have complete object type. */
#ifdef TEST1
struct incomplete;
struct incomplete object = {0}; /* ERROR */
#endif

#ifdef TEST2
union incomplete;
union incomplete object = {0}; /* ERROR */
#endif

#ifdef TEST3
struct incomplete;
extern struct incomplete declaration;
struct incomplete *pointer;
struct complete { int member; } initialized = {0};
int array_of_unknown_size[] = {0};
#endif
