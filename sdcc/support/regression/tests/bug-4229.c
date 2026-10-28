/* bug-4229.c an issue in z80 handling of iy (regalloc vs. codegen) */

#include <testfwk.h>

#pragma opt_code_size

typedef struct {
    unsigned char x, y, direction, timer, mode, hits;
} ActorState;

ActorState actors[6];
volatile unsigned int checksum;

void reproduce(void)
{
    unsigned char index;

    checksum = 0;
    for (index = 0; index < 6; ++index) {
        checksum += actors[index].x + actors[index].y + actors[index].timer;
        checksum += actors[index].mode + actors[index].hits;
    }
}

void testBug(void)
{
    reproduce ();
    ASSERT (checksum == 0);
}

