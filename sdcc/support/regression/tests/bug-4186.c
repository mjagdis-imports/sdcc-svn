/* A z80 codegen bug in the handling of non-dead l in address calculations. */
#include <testfwk.h>

typedef unsigned char u8;
typedef unsigned short u16;
u8* g;
u8 buffer[33];
u8* seen;
void k(u8** p) { seen = *p; }
u8 f(u8 i)
{
    u8* p = &g[i * 32];
    k(&p);
    return *p;
}

void testBug(void)
{
    g = buffer;
    buffer[32] = 42;
    ASSERT(f(1) == 42);
    ASSERT(seen == buffer + 32);
}

