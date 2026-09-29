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

