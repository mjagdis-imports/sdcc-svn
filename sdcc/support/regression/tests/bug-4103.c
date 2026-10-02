/* Bug 4103
   Peephole 36 removes "ld a, #n" without checking whether a is used afterwards.
   On z80 with __sdcccall(1), a byte argument on the stack is pushed from a
   ("push af / inc sp"), so the third argument of set_state() below received a
   stale value of a instead of 1. Peephole 103 first turns
   "ld (hl), #1 / ld a, #1" into "ld a, #1 / ld (hl), a", then peephole 36 drops
   the "ld a, #1".
*/

#include <testfwk.h>
#include <stdint.h>

#if !(defined (__SDCC_mcs51) && defined (__SDCC_MODEL_SMALL)) && !defined(__SDCC_pdk14) // Not enough memory
struct ring {
    uint16_t fault_count;
    uint8_t  pad[5];
    uint32_t tick;
    uint32_t hist[4];
    uint8_t  hist_len;
    uint8_t  hist_head;
    uint8_t  active;
};

struct ring rings[4];

uint8_t seen_id, seen_idx, seen_state;

void set_state(uint8_t id, uint8_t idx, uint8_t state)
{
    seen_id = id;
    seen_idx = idx;
    seen_state = state;
}

void fault(uint8_t id, uint8_t counts)
{
    struct ring *r = &rings[id];

    if (r->fault_count < 0xFFFF)
        r->fault_count++;

    if (counts && !r->active) {
        uint32_t now = r->tick;

        if (r->hist_len == 4 && (uint32_t)(now - r->hist[r->hist_head]) <= 300000UL) {
            r->active = 1;
            set_state(id, 1, 1);
        }

        r->hist[r->hist_head] = now;
        r->hist_head = (uint8_t)((r->hist_head + 1) % 4);
        if (r->hist_len < 4)
            r->hist_len++;
    }
}
#endif

void testBug(void)
{
#if !(defined (__SDCC_mcs51) && defined (__SDCC_MODEL_SMALL)) && !defined(__SDCC_pdk14) // Not enough memory
    rings[2].tick = 1000;
    rings[2].hist_len = 4;
    rings[2].hist_head = 0;
    rings[2].hist[0] = 900;
    seen_state = 0xaa;

    fault(2, 1);

    ASSERT(rings[2].active == 1);
    ASSERT(seen_id == 2);
    ASSERT(seen_idx == 1);
    ASSERT(seen_state == 1);
#endif
}

