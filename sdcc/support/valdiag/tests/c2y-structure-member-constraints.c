int audit_nonempty;

/* N3886 6.7.3.2p3: a structure member has known constant size. */
#ifdef TEST12
struct incomplete;
struct holder { struct incomplete member; /* ERROR */ };
#endif

/* N3886 6.7.3.2p4: bit-field width is an integer constant expression. */
#ifdef TEST13
int width;
struct bits { unsigned field : width; /* ERROR */ };
#endif

/* N3886 6.7.3.2p4: a zero-width bit-field is unnamed. */
#ifdef TEST14
struct bits { unsigned field : 0; /* WARNING(SDCC) */ /* ERROR(GCC) */ };
#endif

/* A flexible-array structure is permitted when it is not an array element. */
#ifdef TEST15
struct flexible { int count; int data[]; };
struct flexible object;
#endif
