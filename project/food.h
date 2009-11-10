/*
** food.h
**
** Written by Peter Sutton
**
** Function prototypes for those functions available externally
*/

/* Guard band to ensure this definition is only included once */
#ifndef FOOD_H
#define FOOD_H

#include <inttypes.h>
#include "position.h"


/* Maximum number of food items that can be on the board
** at any one time.
*/
#define MAX_FOOD 8

/* Each food item that is on the board will have an ID
** between 0 and MAX_FOOD-1 inclusive.
*/

/* init_food()
**
** Initialise the food details and add a few items of food
** (and display them).
*/
void init_food(void); 

/* food_at(position)
** 
** Returns -1 if there is no food at the given position,
** otherwise it returns the food ID of the food at that
** position. The ID is only valid until the next
** remove_food() operation (and can be used as the
** argument to the remove_food() operation).
** (The given position MUST be on the board.)
*/
int8_t food_at(PosnType posn);

/* add_food_items(number)
**
** Attempt to add food items. Returns 
** the number of items actually added. (We may add
** fewer than the number requested because (a) we ran out
** of space to store them, or (b) we can't find space on
** the board for them.) Food items are displayed.
*/
int8_t add_food_items(int8_t numberItems);

/* remove_food(foodID)
**
** Remove a food item from our list of food. This could 
** change the IDs of other food items. The food item is
** removed from the display.
*/
void remove_food(int8_t foodID);

/* 42094353 show_food(void)
**
** Display all the food on the board
*/
void show_food(void);

void hide_food(void);

void blink_food(void);

void food_to_rat(void);

void move_rats(void);

PosnType reverse_direction(int8_t, int8_t);

/* http://www.daniweb.com/code/snippet216329.html by "vegaseat" */
uint16_t rand2(uint16_t);

#endif
