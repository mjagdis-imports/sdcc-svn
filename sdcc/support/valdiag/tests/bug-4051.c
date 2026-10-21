/* Bug #4051: an undefined concatenation macro caused a validator failure.
   The malformed declarations are intentional (GPL-2.0-or-later). */
#include <stdint.h>

#ifdef TEST1C99
#define vdu_declare_tiles(name) \
  extern const uint8_t STRCAT(name,_pixels)[]; /* ERROR (GCC) */ \
  extern const uint8_t STRCAT(name,_colours)[] /* ERROR (GCC) */

void f(void)
{
  vdu_declare_tiles(name); /* ERROR (SDCC) */ /* IGNORE (GCC) */
}
#endif
