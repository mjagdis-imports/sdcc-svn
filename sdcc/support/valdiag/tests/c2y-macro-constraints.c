/* N3886 macro constraints. Each invalid definition is isolated. */
int audit_nonempty;

#ifdef TEST1
/* N3886 6.10.5.1-p3-a: object to function. */
#define M 1 /* IGNORE */
#define M(x) 1 /* WARNING */
#endif

#ifdef TEST2
/* N3886 6.10.5.1-p3-b: object replacement. */
#define M 1 /* IGNORE */
#define M 2 /* WARNING */
#endif

#ifdef TEST3
/* N3886 6.10.5.1-p3-c: function to object. */
#define M(x) 1 /* IGNORE */
#define M 1 /* WARNING */
#endif

#ifdef TEST4
/* N3886 6.10.5.1-p3-d: parameter count. */
#define M(x) 1 /* IGNORE */
#define M(x,y) 1 /* WARNING */
#endif

#ifdef TEST5
/* N3886 6.10.5.1-p3-e: parameter spelling. */
#define M(x) 1 /* IGNORE */
#define M(y) 1 /* WARNING */
#endif

#ifdef TEST6
/* N3886 6.10.5.1-p3-f: function replacement. */
#define M(x) 1 /* IGNORE */
#define M(x) 2 /* WARNING */
#endif

#ifdef TEST7
/* N3886 6.10.5.1-p4: missing space. */
#define M+1 /* WARNING */
#endif

#ifdef TEST8
/* N3886 6.10.5.1-p5-a: argument count. */
#define M(x,y) 1 /* IGNORE */
int value = M(1); /* ERROR */
#endif

#ifdef TEST9
/* N3886 6.10.5.1-p5-b: variadic count. */
#define M(x,y,...) 1 /* IGNORE */
int value = M(1); /* ERROR */
#endif

#ifdef TEST11
/* N3886 6.10.5.2-p3-a: vaopt parenthesis. */
#define M(...) __VA_OPT__ 1 /* ERROR */
#endif

#ifdef TEST12
/* N3886 6.10.5.2-p3-c: vaopt paste. */
#define M(...) __VA_OPT__(## x) /* ERROR */
#endif

#ifdef TEST13
/* N3886 6.10.5.3-p1: stringise. */
#define M(x) # 1 /* ERROR */
#endif

#ifdef TEST14
/* N3886 6.10.5.4-p1-a: paste start. */
#define M ## x /* ERROR */
#endif

#ifdef TEST15
/* N3886 6.10.5.4-p1-b: paste end. */
#define M x ## /* ERROR */
#endif

#ifdef TEST16
/* Valid case: identical replacement lists and corresponding parameter names. */
#define M(x, ...) x __VA_OPT__(+ __VA_ARGS__)
#define M(x, ...) x __VA_OPT__(+ __VA_ARGS__)
int value = M(1, 2);
#endif
