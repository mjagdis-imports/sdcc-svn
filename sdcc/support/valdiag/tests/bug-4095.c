/* bug-4095.c

   Missing error message on negative bit-field size.
 */

#ifdef TEST1
struct bits1 {
  unsigned field : -1; /* ERROR */
};
#endif

#ifdef TEST2
struct bits2 {
  unsigned field : -2; /* ERROR */
};
#endif

#ifdef TEST3
struct bits1u {
  int i;
  unsigned : -1;       /* ERROR */
};
#endif

#ifdef TEST4
struct bits2u {
  int i;
  unsigned : -2;        /* ERROR */
};
#endif

