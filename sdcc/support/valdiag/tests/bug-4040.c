/* bug-4040.c

   A warning about an unused variable at a temporary introduced in inlining that got optimized out later.
 */

#ifdef TEST1

// Based on code by "Under4Mhz" licensed under GPL 2.0 or later

typedef struct {

    int money;

} PlayerData;

PlayerData playerData;                      // Player data

inline PlayerData *PlayerGet( void ) { return &playerData; }

void PlayerMoneyAdd( int amount ) {

    PlayerGet()->money += amount;
}

#endif

