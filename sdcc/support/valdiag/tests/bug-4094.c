/* N3886 6.7.3.2p3: a structure containing a flexible array member shall
   not be an element of an array. */
struct flexible {
  int count;
  int data[];
};

#ifdef TEST1
struct flexible array[2]; /* WARNING */
#endif

#ifdef TEST2
struct flexible object;
#endif

#ifdef TEST3
struct flexible *pointer;
#endif

#ifdef TEST4
struct flexible *pointer_array[2];
#endif

#ifdef TEST5
struct flexible (*pointer_to_invalid_array)[2]; /* WARNING */
#endif
