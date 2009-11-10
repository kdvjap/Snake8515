/*
** position.h
**
** Written by Peter Sutton
**
** 
*/

/* Guard band to ensure this definition is only included once */
#ifndef POSITION_H
#define POSITION_H

#include <inttypes.h>

/* The type that we use for positions. This is an 8 bit type - the
** upper 4 bits holds the x value (column number = 0 on the left, 14 on the right),
** the lower 4 bits holds the y value (0 on the bottom, 6 on the top - different
** to the column number). */
typedef uint8_t PosnType;

/* Functions that can extract the x and y values from a position type */
uint8_t x_position(PosnType posn);
uint8_t y_position(PosnType posn);

/* Function to construct a position from x and y values.
** x and y must be in the range 0 to 15 - they will be 
** masked to this range (i.e. lowest 4 bits of each used).
*/
PosnType position(uint8_t x, uint8_t y);

#endif
