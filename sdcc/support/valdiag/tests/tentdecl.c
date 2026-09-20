
#ifdef TEST0
int a;
int a;
#endif

#ifdef TEST1
int a;		/* IGNORE */
char a;		/* ERROR */
#endif

#ifdef TEST2
int a;		/* IGNORE */
int *a;		/* ERROR */
#endif

#ifdef TEST3
int *a;		/* IGNORE */
int a[5];	 /* ERROR */
#endif

/* array size must match */

#ifdef TEST4
int a[4];	/* IGNORE */
int a[5];	/* ERROR */
#endif

/* but it is legal to clarify */
/* an incomplete type */

#ifdef TEST4b
int a[];
int a[5];
#endif

/* type qualifier must match */

#ifdef TEST5
int a;		/* IGNORE */
volatile int a; /* ERROR */
#endif

#ifdef TEST6
int a;		/* IGNORE */
const int a;	/* ERROR */
#endif

#ifdef TEST7
int a=1;	/* IGNORE */
int a=2;	 /* ERROR */
#endif

#ifdef TEST7a
int a=1;
int a;
#endif

#ifdef TEST8
int a=1;	/* IGNORE */
int a=1;	/* ERROR */
#endif

#if defined(__SDCC)
#define AT(x) __at x
#else
#define AT(x)
#endif

#if defined(__has_data)
#define DATA __data
#else
#define DATA
#endif

#if defined(__has_xdata)
#define XDATA __xdata
#else
#define XDATA
#endif

#ifdef TEST9
XDATA int a;	/* IGNORE */
DATA int a;	/* ERROR(__has_data || __has_xdata) */
#endif

#ifdef TEST9b
DATA int a;	/* IGNORE */
XDATA int a;	/* ERROR(__has_data || __has_xdata) */
#endif

#ifdef TEST9c
extern DATA int a;
DATA int a;
#endif

#ifdef TEST9d
extern XDATA int a;
XDATA int a;
#endif

#ifdef TEST9e
extern XDATA int a; /* IGNORE */
DATA int a;		/* ERROR(__has_data || __has_xdata) */
#endif

#ifdef TEST9f
extern DATA int a; /* IGNORE */
XDATA int a;		/* ERROR(__has_data || __has_xdata) */
#endif

#ifdef TEST10
extern volatile XDATA AT(0) int a; /* IGNORE */
volatile XDATA int a;	/* ERROR(SDCC) */
#endif
