/* Flexible-array containment restrictions also pass through unions. */
struct Flexible { int count; char data[]; };
union Payload { struct Flexible flexible; int number; };
union Nested { union Payload payload; long number; };
union Deep { union Nested nested; char byte; };

#ifdef TEST1
struct Container { union Payload value; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST2
union Payload values[2]; /* WARNING */
#endif

#ifdef TEST3
struct Container { union Nested value; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST4
union Nested values[2]; /* WARNING */
#endif

#ifdef TEST5
struct Container { union Deep value; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST6
union Deep values[2]; /* WARNING */
#endif

#ifdef TEST7
union Payload (*pointer)[2]; /* WARNING */
#endif

#ifdef TEST8
extern union Nested values[]; /* WARNING */
#endif

#ifdef TEST9
typedef union Payload Array[2]; /* WARNING */
#endif

#ifdef TEST10
struct Container { union Payload values[2]; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST11
union Container { union Payload values[2]; }; /* WARNING */
#endif

#ifdef TEST12
union Container { struct Flexible values[2]; }; /* WARNING */
#endif

#ifdef TEST13
struct Container { int prefix; union Nested value; int suffix; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST14
union Payload value;
#endif

#ifdef TEST15
union Nested value;
#endif

#ifdef TEST16
union Deep value;
#endif

#ifdef TEST17
union Payload *pointer;
#endif

#ifdef TEST18
union Nested *pointers[2];
#endif

#ifdef TEST19
struct Container { union Deep *pointer; };
#endif

#ifdef TEST20
union Pointer { struct Flexible *pointer; int number; };
struct Container { union Pointer value; };
union Pointer values[2];
#endif

#ifdef TEST21
struct Fixed { int count; char data[3]; };
union FixedPayload { struct Fixed fixed; int number; };
union FixedNested { union FixedPayload payload; long number; };
struct Container { union FixedNested value; };
union FixedNested values[2];
#endif

#ifdef TEST22
struct Container { union { struct Flexible flexible; int number; }; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST23
union Anonymous { union { struct Flexible flexible; int number; }; long other; };
struct Container { union Anonymous value; }; /* WARNING */ /* ERROR(SDCC) */
#endif

#ifdef TEST24
union Payload (*function(void))[2]; /* WARNING */
#endif

#ifdef TEST25
union Container { union Payload (*pointer)[2]; }; /* WARNING */
#endif

#ifdef TEST26
union Permitted { union Payload value; int number; };
struct Container { union Permitted *pointer; };
#endif

