/*
** position.c
**
** Written by Peter Sutton
**
** 
*/

#include "position.h"

/* Functions that can extract the x and y values from a position type */
uint8_t x_position(PosnType posn) {
	//return (posn >> 4) & 0x0F;	//4209435
	/* rats will be identified by bit 4 */
	return ((posn >> 4) & 0x07);
}

uint8_t y_position(PosnType posn) {
	return (posn & 0x0F);
}

/* Function to construct a position from x and y values.
*/
PosnType position(uint8_t x, uint8_t y) {
	return ((x & 0x0F) << 4) | (y & 0x0F);
}
