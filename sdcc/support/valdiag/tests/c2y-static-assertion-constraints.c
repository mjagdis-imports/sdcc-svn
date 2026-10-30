int audit_nonempty;

#ifdef __SDCC
#pragma std_c11
#endif
/* N3886 6.5.4.6p2: the assertion expression is an integer constant
   expression, even if its runtime value might be nonzero. */
#ifdef TEST3
int nonconstant_value;
_Static_assert(nonconstant_value, "not constant"); /* ERROR */
#endif
