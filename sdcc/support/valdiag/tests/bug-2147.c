/* Bug #2147: excess scalar initialisers crashed PIC14 glue generation. */
#include <stdint.h>

#ifdef TEST1C99
uint8_t arr = {6, 9, 8, 7, 6}; /* WARNING */
#endif
