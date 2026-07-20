/* bug-4039.c
   An issue in the use of partially-used (by register allocation) iy for cached temporary addresses (by codegen).
 */

#include <testfwk.h>

// Based on code by "Under4Mhz" licensed under GPL 2.0 or later

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined (__SDCC_z80) || defined (__SDCC_z80n) || defined (__SDCC_ez80)
static volatile __sfr __at 0x00 VDPControlPort;
static volatile __sfr __at 0x01 VDPDataPortOut;
#else
static volatile unsigned char VDPControlPort;
static volatile unsigned char VDPDataPortOut;
#endif

bool vdu_interrupt_state;
#define VDU_ADDRESS_SET_MASK 0
#define BYTE_HI(x) ((x)>>8)

#define vdu_interrupt_enable_fast() { extern bool vdu_interrupt_state; vdu_interrupt_state = true;  }
#define vdu_interrupt_disable_fast() { extern bool vdu_interrupt_state; vdu_interrupt_state = false; }
#define vdu_address_set_unsafe(address) { VDPControlPort=address; VDPControlPort=BYTE_HI((address) | VDU_ADDRESS_SET_MASK ); }
#define vdu_address_set_fast(address) { vdu_interrupt_disable_fast(); vdu_address_set_unsafe(address); vdu_interrupt_enable_fast(); }
#define vdu_address_set_fast2(address) { vdu_interrupt_disable_fast(); vdu_address_set_unsafe(address); check(); vdu_interrupt_enable_fast(); }

void check(void)
{
    ASSERT( !vdu_interrupt_state );
}

inline void TileSet( uint16_t addr, uint8_t id ) {

    id *= 4;
    addr += 0x1000;

    vdu_address_set_fast( addr );
    addr += 32;

    vdu_address_set_fast2( addr );
}

inline void DrawSprite( uint16_t addr, uint8_t id ) {

    TileSet( addr, id );
}

void LevelDraw() {

    uint8_t row = 2;
    uint16_t addr = 32;

    do {

        int current = 0;
        uint8_t col = 2;

        do {
            DrawSprite( addr, current );
        } while( --col );

        addr += 32;

    } while (--row);
}

void
testBug(void) {
  
    LevelDraw();
}

