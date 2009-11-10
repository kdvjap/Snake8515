/*
** food.c
**
** Written by Peter Sutton
**
*/

#include "position.h"
#include "food.h"
#include "board.h"
#include "snake.h"

/*
** Global variables.
** foodPositions records the (x,y) positions of each food item.
** numFoodItems records the number of items. The index into the
** array is the food ID. Food IDs will range between 0 and 
** numFoodItems-1. If we delete a food item, we rearrange the 
** array.
**
*/
PosnType foodPositions[MAX_FOOD];
int8_t numFoodItems;

/* 
** Initialise food details - three to start with. (It is assumed
** that only the snake is on the display before this.)
*/
void init_food(void) {
	numFoodItems = 0;
     
    /* Add some food */
    add_food_items(3);    
}

/* Returns a food ID if there is food at the given position,
** otherwise returns -1
*/
int8_t food_at(PosnType posn) {
    int8_t id;
    for(id=0; id < numFoodItems; id++) {
        if(foodPositions[id] == posn) {
            /* Food found at this position */
            return id;
        }
    }
    /* No food found at the given position */
    return -1;
}

/* Attempt to add food items. Returns 
** the number of items actually positioned. (We may place
** fewer than the number requested because (a) we ran out
** of space to store them, or (b) we can't find space on
** the board for them.)
*/
int8_t add_food_items(int8_t num) {
    /* Generate random positions until we get one
    ** at which there is no snake and no existing
    ** food.
    */
    int8_t x, y, i, attempts;
    x = 0;
	y = 0;
    for(i=0; i < num; i++) {
        /* First check that we have space in our list
        ** of food items to hold another one. If not, 
        ** just return.
        */
        if(numFoodItems >= MAX_FOOD) {
            /* Number we've managed to add is i */
            return i;
        }
        
        /* Make some attempts to position 
        ** some food. (We don't try forever - just
        ** in case the board is filled.) We start
		** at (0,0) and generate a series of 
		** positions.
        */
        attempts = 0;
	
        do {
            x = (x+2)%BOARD_WIDTH;
            y = (y+2)%BOARD_ROWS;
            attempts++;
        } while(attempts < 100 && 
                (is_snake_at(position(x,y)) || food_at(position(x,y)) != -1));
        
        if(attempts >= 100) {
            /* We tried 100 times to generate a position
            ** but they were all occupied.
            */
            return i;
        }
        
        /* Now have an x,y position where we can put
        ** food. (There is no food or snake at this position.)
        */
        foodPositions[numFoodItems] = position(x,y);
        numFoodItems++;
        add_food_item_to_board(position(x,y));
    }
    return num;
}

/*
** Remove the food item from our list of food (and
** from the display if there is no snake at that position)
*/
void remove_food(int8_t foodID) {
    int8_t i;
        
    if(foodID < 0 || foodID >= numFoodItems) {
        /* Invalid foodID */
        return;
    }
        
    /* Remove the food from the display.
    */
	remove_food_item_from_board(foodPositions[foodID]);
     
    /* Shuffle our list of food items along so there are
	** no holes in our list 
	*/
    for(i=foodID+1; i <numFoodItems; i++) {
        foodPositions[i-1] = foodPositions[i];
    }
    numFoodItems--;
}

