/*
** board.c
**
** Written by Peter Sutton
**
** Keeps the displayed board updated.
*/

#include "position.h"
#include "board.h"
#include "led_display.h"
#include "score.h"
#include "snake.h"
#include "food.h"
#include <stdio.h>

/* Functions available within this file */
static void turn_on_led_at(int8_t x, int8_t y);
static void turn_off_led_at(int8_t x, int8_t y);

/* Initialise board - initial snake and some food. It is
** assumed the display is blank when this function is called.
*/
void init_board(void) {
    /* Cause the snake to be reset */
    init_snake();
    
    /* Add some food */
    init_food();
}

/* Remove snake element from the board. 
**
** This function will turn the LED off - unless there
** is food at that position
*/
void remove_snake_element_from_board(PosnType posn) {
	if(food_at(posn) == -1) {
		turn_off_led_at(x_position(posn), y_position(posn));
	}
}

/* Add snake element to the board.
**
** This function will turn on the LED at the given
** position.
*/
void add_snake_element_to_board(PosnType posn) {
	turn_on_led_at(x_position(posn), y_position(posn));
}

/* Remove food item from the board. 
**
** This function will turn the LED off - unless there
** is a snake at that position.
*/
void remove_food_item_from_board(PosnType posn) {
	if(!is_snake_at(posn)) {
		turn_off_led_at(x_position(posn), y_position(posn));
	}
}

/* Add food item to the board.
**
** This function will turn on the LED at the given position.
*/
void add_food_item_to_board(PosnType posn) {
	turn_on_led_at(x_position(posn), y_position(posn));
}


/* Returns true (1) if the given x,y position is off
** the valid board area, false(0) otherwise.
*/
int8_t is_off_board(int8_t x, int8_t y) {
    if(x < 0 || x >= BOARD_WIDTH) {
        return 1;
    } 
    if(y < 0 || y >= BOARD_ROWS) {
        return 1;
    }
    return 0;
}


/* Private functions - that access the LED display array directly */
static void turn_on_led_at(int8_t x, int8_t y) {
	uint8_t row;
	uint8_t colBitPosn;

	row = x;
	colBitPosn = y;

	display[row] |= (1<<colBitPosn);
}

static void turn_off_led_at(int8_t x, int8_t y) {
	uint8_t row;
	uint8_t colBitPosn;

	row = x;
	colBitPosn = y;
	display[row] &= ~(1<<colBitPosn);
}
