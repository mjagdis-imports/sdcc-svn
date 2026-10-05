/* N3886 6.5.3.2p2: when one operand has array type, the other shall not
   be a negative integer constant expression. */

int array[3];

#ifdef TEST1
int first(void) { return array[-1]; } /* WARNING */
#endif

#ifdef TEST2
int second(void) { return (-1)[array]; } /* WARNING */
#endif

#ifdef TEST3
int *pointer = array + 1;
int through_pointer(void) { return pointer[-1]; }
#endif

#ifdef TEST4
int index;
int nonconstant(void) { return array[index]; }
#endif

#ifdef TEST5
/* On ds390, p[-1] reads four bytes at a byte offset of -4. The upper size
   bound for an unknown pointer used to wrap to 3 on a 32-bit host. */
long through_ptr_to_long(long *p) { return p[-1]; }
#endif

#ifdef TEST6
void through_pointer_store(int *p) { p[-1] = 1; }
#endif
