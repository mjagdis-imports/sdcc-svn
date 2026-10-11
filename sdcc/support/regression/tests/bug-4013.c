/* bug-4013.c
   __at gives the function pointer a fixed address. It should not make
   SDCC warn about a qualified function return type.
 */

#include <testfwk.h>

__at(0xff81) void (*f)(void);
void (*__at(0xff81) g)(void);

typedef void (*ftype)(void);
ftype __at(0xff81) h;

void
testBug (void)
{
}
