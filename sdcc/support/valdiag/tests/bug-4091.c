/* bug-4095.c

   Missing error message on sizeof of bit-field.
 */

#ifdef TEST1
struct bits { unsigned field : 12; } bits;

enum { invalid_bitfield_size = sizeof bits.field }; /* ERROR */
#endif

#ifdef TEST2
struct bits { unsigned field : 12; } bits;

int j = sizeof bits.field; /* ERROR */
#endif
