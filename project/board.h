/*
** board.h
**
** Written by Peter Sutton
**
** Function prototypes for those functions available externally
*/

/* Guard band to ensure this definition is only included once */
#ifndef BOARD_H
#define BOARD_H

#include <inttypes.h>
#include "position.h"

/*
** The board is 15 rows in size with 7 columns. (The board row
** and column is different to the display row and column.)
*/
#define BOARD_ROWS 15
#define BOARD_WIDTH 7

/*
** Initialise the board. This will reset all 
** the variables that contain board information
** and place an initial snake on the board. It is 
** assumed the display is blank when this function
** is called. The initial snake and food items will
** be added to the board.
*/
void init_board(void); 

/*
** Return true if given position (x,y) is not a valid 
** board position.
*/
int8_t is_off_board(int8_t x, int8_t y);

/*
** Add/remove a block of the snake to the board at
** the given (x,y) position. (Updates display.)
*/
void remove_snake_element_from_board(PosnType posn);
void add_snake_element_to_board(PosnType posn);

/*
** Add/remove a food item to the board at the given
** (x,y) position. (Updates display.)
*/
void remove_food_item_from_board(PosnType posn);
void add_food_item_to_board(PosnType posn);

//4209435
/* redraws the board after it has been cleared */
void render_board(void);

#endif
