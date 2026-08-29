/* bug-4054.c
   ?
 */

#include <testfwk.h>

#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t value;
    uint8_t value2;
} A;

typedef struct {
    uint8_t value;
    uint8_t value2;
} B;

volatile int memcmp_result;

void check_cast(B actual, B expected) {

    memcmp_result = memcmp(&actual, &expected, sizeof(B)); 
}

void
testBug(void) {
#if 0 // Bug not yet fixed
    A a = { .value = 15, .value2 = 5 };
    B b = {.value = 15, .value2 = 5 };
    check_cast(*(B*)&a, b); // memcmp_result != 0 (bug)
    ASSERT (!memcmp_result);
    
    B var = *(B*)&a;
    check_cast(var, b); // memcmp_result == 0
    ASSERT (!memcmp_result);
#endif
}

