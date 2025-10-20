// A lot of undefined behavior from C23 has been removed in C2y
// Martin Uecker's "Ghosts and Demons" gives a good overview.
// The numbering of the test cases below corresponds to the numbering therein.
// In many cases, the resolutionw as to make it a constraint violation.

#ifdef TEST87
struct foo;
static struct foo x; /* ERROR */
#endif

