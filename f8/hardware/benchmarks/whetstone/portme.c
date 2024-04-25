#include <stdio.h>
#include <signal.h>
#include <stdint.h>

#define IRQCTRLEN (*((volatile uint8_t *)0x0010))
#define IRQCTRLACT (*((volatile uint8_t *)0x0012))

#define TIMER0CTRL (*((volatile uint8_t *)0x0018))
#define TIMER0CNT (*((volatile uint16_t *)0x001a))
#define TIMER0RLD (*((volatile uint16_t *)0x001c))

#define GPIO2DDR (*((volatile uint8_t *)0x0038))
#define GPIO2ODR (*((volatile uint8_t *)0x003a))

unsigned long counter;

volatile unsigned char sendcounter;
volatile unsigned int senddata;
volatile sig_atomic_t counter_updated, sending;

void interrupthandler(void) __interrupt(0)
{
	counter++;
	counter_updated = 1;

	if(sending)
	{
		GPIO2ODR = senddata & 1;
		senddata >>= 1;

		if(!--sendcounter)
			sending = 0;
	}

	IRQCTRLACT = 0; // Clear interrupt request.
}

void init(void)
{
	// Set up GPIO2
	GPIO2ODR = 0x01;
	GPIO2DDR = 0x01;

	// Set up TIMER0 for 9.6 KHz interrupt assuming 2 MHz system clock.
	TIMER0CNT = 65328;
	TIMER0RLD = 65328;
	TIMER0CTRL = 0x01; // Start timer.
	IRQCTRLEN = 0x01;  // Enable timer 0 overflow interrupt.
}

unsigned long clock(void)
{
	counter_updated = 0;
	unsigned long ret;
	do
		ret =  counter;
	while(counter_updated);
	return(ret / 96); // Whetstone expects clock() to return milliseconds.
}

int putchar(int c)
{
	while(sending);

	senddata = (c << 1) | 0x200;

	sendcounter = 10;

	sending = 1;

	return (c);
}

