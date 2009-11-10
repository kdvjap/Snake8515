/*
** terminalio.h
**
** Functions for interacting with the terminal. These should be used
** to encapsulate all sending of escape sequences.
**
** Written by Peter Sutton,
*/

/* Guard band to ensure this definition is only included once */
#ifndef TERMINALIO_H
#define TERMINALIO_H

#include <inttypes.h>

/*
** x and y are measured relative to the top left of the screen. First
** column is 1, first row is 1.
**
** The display parameter is a number between 0 and 47 (not all values
** are valid).
*/

/* Move the cursor to line y, row x (as measured from the top left) */
void move_cursor(int8_t x, int8_t y);

/* Return display to normal mode (i.e. not blinking/reverse video etc) */
void normal_display_mode(void);

/* Enable reverse video mode */
void reverse_video(void);

/* Clear the terminal display */
void clear_terminal(void);

/* Clear the current line, from the current position to the end of the line */
void clear_to_end_of_line(void);

/* Set the given display mode, where parameter is a number from 0 to 47.
** See the list of display attributes on the terminal IO webpage
*/
void set_display_attribute(int8_t parameter);

/*
** Draw a reverse video line on the terminal. startx must be <= endx.
** starty must be <= endy
*/
void draw_horizontal_line(int8_t y, int8_t startx, int8_t endx);
void draw_vertical_line(int8_t x, int8_t starty, int8_t endy);

#endif
