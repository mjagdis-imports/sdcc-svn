/* pdk.src/instcl.h */

int get_mem(unsigned int addr);
unsigned char add_to(unsigned char initial, int value);
unsigned char sub_to(unsigned char initial, int value);
unsigned char get_io(t_addr addr);
void store_io(t_addr addr, unsigned char value);

/* End of pdk.src/instcl.h */
