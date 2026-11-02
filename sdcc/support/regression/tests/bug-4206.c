/* bug-4206.c an issue in z80 addition codegen */

#include <testfwk.h>

typedef unsigned char u8;
typedef unsigned long u32;

void f(u32 entry, u8* ptr, u8* out, const u8* order)
{
    for(u8 idx = entry; idx < 16; ++idx)
    {
        *out++ = ptr[*order++];
    }
}

u8 order[16] = {0, 1, 2, 1, 0};
u8 out[16];
u8 ptr[3] = {0, 1, 2};
    
void testBug(void)
{
    f(0, ptr, out, order);

    ASSERT(out[0] == 0);
    ASSERT(out[1] == 1);
    ASSERT(out[2] == 2);
    ASSERT(out[3] == 1);
    ASSERT(out[4] == 0);
}

