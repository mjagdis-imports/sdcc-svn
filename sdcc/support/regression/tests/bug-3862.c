/* Bug #3862: a compound literal using inline parameters (GPL-2.0-or-later).
 */
#include <testfwk.h>

typedef struct
{
  unsigned char x;
  unsigned char y;
} maths_point_u8;

typedef struct
{
  int x;
  int y;
} maths_point_i8;

/* Also supply an external definition when the host does not inline the call. */
extern maths_point_u8 *PositionToTile(const maths_point_i8 *position, maths_point_u8 *screen);

inline maths_point_u8 *PositionToTile(const maths_point_i8 *position, maths_point_u8 *screen)
{
  *screen = (maths_point_u8){position->x * 2, position->y * 2};
  return screen;
}

void
testBug(void)
{
  maths_point_i8 position = {1, 2};
  maths_point_u8 screen;

  ASSERT (PositionToTile(&position, &screen) == &screen);
  ASSERT (screen.x == 2);
  ASSERT (screen.y == 4);
}
