/* N3886 6.5.4.5p1: sizeof shall not be applied to a function type. */
int function(void);
typedef int function_type(void);

#ifdef TEST1
enum { expression_size = sizeof(function) }; /* ERROR */
#endif

#ifdef TEST2
enum { type_name_size = sizeof(function_type) }; /* ERROR */
#endif

#ifdef TEST3
enum { function_pointer_size = sizeof(&function) };
#endif

#ifdef TEST4
static int (*function_pointer)(void) = &function;
#endif
