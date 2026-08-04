/* bug-4047.c
   A bug in z80 addition codegen.
 */

#include <testfwk.h>

// Based on code by "Under4Mhz" licensed under GPL 2.0 or later

#include <stdint.h>
#include <stdbool.h>

#define COLS 8
#define ROWS 3

typedef struct { int8_t row; int8_t col; uint8_t orientation : 1; } Shape;
typedef struct { uint8_t sprite; } TableItem;

static TableItem table[ROWS][COLS];

TableItem *TableGet( uint8_t col, uint8_t row ) { return &table[row][col]; }

///< Check next position valid. All three conditions are needed to trigger.
int BlockMoveCheck( const Shape *shape ) {

    bool vertical = shape->orientation == 0;
    uint8_t height = vertical;
    uint8_t width = !vertical;

    if ( shape->col < 0 || shape->col + width >= COLS || shape->row + height >= ROWS ) return false; // codegen bug affected one of the additions used in a comparison here.
    else if ( TableGet( shape->col, shape->row )->sprite ) return false;
    else if ( TableGet( shape->col + width, shape->row + height )->sprite ) return false;

    return true;
}

void
testBug (void)
{
   Shape s;

   s.row = 0;
   s.col = 0;
   s.orientation = 1;
   ASSERT ( BlockMoveCheck (&s));

   s.row = 0;
   s.col = -1;
   s.orientation = 1;
   ASSERT ( !BlockMoveCheck (&s));

   s.row = 0;
   s.col = 8;
   s.orientation = 1;
   ASSERT ( !BlockMoveCheck (&s));

   s.row = 3;
   s.col = 0;
   s.orientation = 1;
   ASSERT ( !BlockMoveCheck (&s));
}

