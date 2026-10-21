/* Bug #3408: a local structure in an aggregate initialiser (GPL-2.0).
 */
#include <testfwk.h>
#include <stdint.h>

typedef struct
{
  int8_t x;
  int8_t y;
} maths_point_i8;

void
testBug(void)
{
  maths_point_i8 dirx = {0, 0};
  maths_point_i8 directionsy[1] = {dirx};

  ASSERT (directionsy[0].x == 0);
  ASSERT (directionsy[0].y == 0);
}
