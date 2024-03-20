// A 5 bit LED second counter.

#include <stdint.h>

#define IRQCTRLEN (*((volatile uint8_t *)0x0010))
#define IRQCTRLACT (*((volatile uint8_t *)0x0012))

#define TIMER0CTRL (*((volatile uint8_t *)0x0018))
#define TIMER0CNT (*((volatile uint16_t *)0x001a))
#define TIMER0RLD (*((volatile uint16_t *)0x001c))

#define GPIO0DDR (*((volatile uint8_t *)0x0028))
#define GPIO0ODR (*((volatile uint8_t *)0x002a))

uint16_t counter = 64536;

void interrupthandler(void) __interrupt(0)
{
	counter++;
	if(!counter)
	{
		counter = 64536;
		GPIO0ODR++; // Increment second counter on LEDs once per second.
	}
	IRQCTRLACT = 0; // Clear interrupt request.
}

void main(void)
{
	// Set up GPIO0
	GPIO0ODR = 0;    // Switch off all LEDs.
	GPIO0DDR = 0x1f; // Set the lines connected to LEDs as output.

	// Set up TIMER0 for 1 KHz interrupt assuming 3 MHz system clock.
	TIMER0CNT = 62536;
	TIMER0RLD = 62536;
	// Set up TIMER0 for 1 KHz interrupt assuming 2.5 MHz system clock.
	//TIMER0CNT = 65276;
	//TIMER0RLD = 65276;
	IRQCTRLEN = 0x01;  // Enable timer 0 overflow interrupt.
	TIMER0CTRL = 0x01; // Start timer.

	for(;;);
}

