/* bug-4053.c
   Codegen for z80-related targets would generate wrong code for reads from
   bit-fields of type bool (sign-extending as if they were int:1 bit-fields).
 */

#include <testfwk.h>

#include <stdbool.h>

#define ICON_CBX            "\x18"
#define ICON_CBX_CHECKED    "\x19"

typedef struct test_options_t {
    bool print_fast                 : 1;
    bool fancy_sgb_border           : 1;
    bool show_grid                  : 1;
    bool save_confirm               : 1;
    bool ir_remote_shutter          : 1;
    bool boot_to_camera_mode        : 1;
    bool double_speed               : 1;
    unsigned char shutter_timer;
} test_options_t;

test_options_t test_state;
unsigned char text_buffer_test[4];
const unsigned char * const checkbox[] = {ICON_CBX, ICON_CBX_CHECKED};

void test_array_index_bitpacked_struct_bool(void) {

        const unsigned char * temp =  checkbox[test_state.save_confirm];
        *text_buffer_test = *temp;
}

void
testBug(void) {
    test_state.save_confirm = true;
    test_array_index_bitpacked_struct_bool();
    ASSERT (text_buffer_test[0] == ICON_CBX_CHECKED[0]);

    test_state.save_confirm = false;
    test_array_index_bitpacked_struct_bool();
    ASSERT (text_buffer_test[0] == ICON_CBX[0]);
}

