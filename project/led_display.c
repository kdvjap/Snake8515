/*
** FILE: led_display.c
**
** Written by Peter Sutton.
**
** Module that implements our LED display.
**
*/

#include "led_display.h"
#include <avr/io.h>

/* Global variable - see comment in header file */
volatile uint16_t display[NUM_ROWS];

void init_display(void) {
	uint8_t i;

	/* Set ports A and B to be outputs (except most significant
	 * bit of port A) */
	DDRA |= 0x7F;
	DDRB |= 0xFF;

	/* Set 3 least significant bits of C to be outputs */
	DDRC |= 0x07;

	/* Empty the display */
	for(i=0; i<NUM_ROWS; i++) {
		display[i] = 0;
	}
}

void display_row(void) {	
	/* Keep track of the row number we're up to. ("static" 
	 * indicates that the variable value will be remembered 
	 * from one function execution to the next.)
	 */
	static uint8_t row = 0;

	/* Increment our row number (and wrap around if necessary) */
	if(++row == NUM_ROWS) {
		row = 0;
	}

	/* Output our row number to port C. This assumes the other 
	 * bits of port C are not being used. If they are, then
	 * this line of code needs to be changed.
	 */
	PORTC = row;

	/* Output the correct row data to ports A and B. (Port B gets
	 * the high byte, port A gets the low byte.) We need to invert
	 * the data since we need a low output for the LED to be lit. 
	 * Note - most significant bit is not displayed/used.
	 */
	PORTB = ~(uint8_t)(display[row] & 0xFF);
	PORTA = ~(uint8_t)((display[row] >> 8)& 0X7F);
}
