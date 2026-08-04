/* bug-4046.c
   A missing condition check in a peephole rule.
 */

#include <testfwk.h>

// Based on code by "Under4Mhz" licensed under GPL 2.0 or later

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool CacheUpdate( uint8_t *cache, uint8_t frame, uint8_t position );

uint8_t cacheGear;

void GearShow( uint8_t gear ) {

    if ( gear == 0 ) {

        if ( CacheUpdate( &cacheGear, 0, gear ) ) { // Peephole optimizer introduced an invalid "ld hl, a" instruction around here.

        }
    }
}

void
testBug(void) {
  
    GearShow ( 0 );
}

#pragma disable_warning 85
bool CacheUpdate( uint8_t *cache, uint8_t frame, uint8_t position )
{
}

