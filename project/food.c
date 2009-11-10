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
//4209435
#include "wall.h"
#include "timer.h"
#include "score.h"

//for debugging
#include "led_display.h"
#include "terminalio.h"
#include <stdio.h>
#include <avr/pgmspace.h>
//

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
int8_t currRatDirection[2];
int8_t numFoodItems;

/* 
** Initialise food details - three to start with. (It is assumed
** that only the snake is on the display before this.)
*/
void init_food(void) {
	numFoodItems = 0;
     
    /* Add some food */
    add_food_items(3);

	//4209435
	/* Transform some food to rats */
	food_to_rat();
	food_to_rat();
}

/* Returns a food ID if there is food at the given position,
** otherwise returns -1
*/
int8_t food_at(PosnType posn) {
    int8_t id;
    for(id=0; id < numFoodItems; id++) {
        if(foodPositions[id] == (posn | 0x80) || foodPositions[id] == posn) {
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
        } while(
				attempts < 100 && 
                (
					is_snake_at(
						((x & 0x0F) << 4 ) | (y & 0x0F)
					) || 
					food_at(
						((x & 0x0F) << 4) | (y & 0x0F)
					) != -1 || 
					is_wall_at(
					((x & 0x0F) << 4) | (y & 0x0F)
					)
				)
			);
        
        if(attempts >= 100) {
            /* We tried 100 times to generate a position
            ** but they were all occupied.
            */
            return i;
        }
        
        /* Now have an x,y position where we can put
        ** food. (There is no food or snake at this position.)
        */
        foodPositions[numFoodItems] = ((x & 0x0F) << 4) | (y & 0x0F);
        numFoodItems++;
        add_food_item_to_board(((x & 0x0F) << 4) | (y & 0x0F));
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

	//add food
	add_food_items(1);
    //player must have eaten the food, update score
	if(foodPositions[foodID] & 0x80){
		add_to_score(10);
		//add another rat
		food_to_rat();
	}
	else
		add_to_score(5);
	
	update_score();
    /* Remove the food from the display.
    */
	remove_food_item_from_board(foodPositions[foodID] & 0x7F);
     
    /* Shuffle our list of food items along so there are
	** no holes in our list 
	*/
    for(i=foodID+1; i <numFoodItems; i++) {
        foodPositions[i-1] = foodPositions[i];
    }
    numFoodItems--;
}

//4209435
void show_food(void) {
	int8_t i;
	for(i=0; i < numFoodItems; i++) {
		add_food_item_to_board(foodPositions[i]);
	}
}

//4209435
void hide_food(void) {
	int8_t i;
	//first two foods are always rats
	for(i=2; i < numFoodItems; i++) {
		remove_food_item_from_board(foodPositions[i]);

		/* for debug
		move_cursor(1,20);
		printf_P(PSTR("Food Positions:"));
		int8_t i;
		for(i = 0; i < MAX_SNAKE_SIZE; i++)	
			printf_P(PSTR(" %u"), foodPositions[i] );
		*/	
	}
}

//4209435
void blink_food(void){
	/*
	** status = 0, light is on
	** status = 1, light is off
	*/
	static uint8_t status = 0;

	if(status) {
		show_food();
		status = 0;
	}
	else {
		hide_food();
		status = 1;
	}
}

//4209435
void food_to_rat(void){
	//make the first two food items always rats
	//try to make the first a rat
	//if it is already, make the second a rat
	//Note: take advantage of the fact that maxX is 6
	if(!(foodPositions[0] & 0x80)){
		foodPositions[0] |= 0x80;
		currRatDirection[0] = rand2(256) % 4;
	}
	else{
		foodPositions[1] |= 0x80;	
		currRatDirection[1] = rand2(256) % 4;
	}
		

	/*for debugging *
	printf_P(PSTR(" %x"), foodPositions[0] & 0x80 );
	wait_for(1000);
	//*/
}

void move_rats(void){
	int8_t i;
	int8_t direction = 0;
	int8_t ratMask;
	PosnType ratPosition;

	for(i = 0; i < 2; i++){
		ratPosition = foodPositions[i];
		ratMask = ratPosition & 0x7F;
		direction = rand2(65000) % 4;
		
		remove_food_item_from_board(ratPosition);
		switch(direction){
			case UP:
				ratPosition += 0x01;
				if(( ratPosition & 0x0F) >= BOARD_ROWS || is_snake_at(ratMask) || food_at((ratMask) + 0x01) != -1 || is_wall_at((ratMask) + 0x01))
					ratPosition = reverse_direction(UP, i);
				break;
			case DOWN:
				ratPosition -= 0x01;
				if((ratPosition & 0x0F) <= 0 || is_snake_at(ratMask) || food_at((ratMask) - 0x01) != -1 || is_wall_at((ratMask) + 0x01))
					ratPosition = reverse_direction(DOWN, i);
				break;
			case LEFT:
					ratPosition -= 0x10;
				if((ratPosition & 0x70) <= 0 || is_snake_at(ratMask) || food_at((ratMask) - 0x10) != -1 || is_wall_at((ratMask) + 0x01))
					ratPosition = reverse_direction(LEFT, i);
				break;
			case RIGHT:
				ratPosition += 0x10;
				if(((ratPosition>>4) & 0x07) >= BOARD_WIDTH - 1 || is_snake_at(ratMask) || food_at((ratMask) + 0x10) != -1 || is_wall_at((ratMask) + 0x01))
					ratPosition = reverse_direction(RIGHT, i);
				break;
		}
		
		currRatDirection[i] = direction;
		foodPositions[i] = ratPosition;

	/*for debugging *
	move_cursor(1,16);
	printf_P(PSTR("direction: %u"), direction );
	//*/
	}
}

PosnType reverse_direction(int8_t direction, int8_t i){
	int8_t currDirection;
	PosnType ratPosition;
	currDirection = currRatDirection[i];
	ratPosition = foodPositions[i];

	switch(direction){
		case UP:
				currDirection = DOWN;
				ratPosition -= 0x01;
			break;
		case DOWN:
				currDirection = UP;
				ratPosition += 0x01;
			break;
		case LEFT:
				currDirection = RIGHT;
				ratPosition += 0x10;
			break;
		case RIGHT:
				currDirection = LEFT;
				ratPosition -= 0x10;
			break;
	}
		
	currRatDirection[i] = direction;
	return ratPosition;
}

/* http://www.daniweb.com/code/snippet216329.html by "vegaseat" */
uint16_t rand2(uint16_t lim){
    static uint16_t a = 1;  // could be made the seed value
	a = (a * 32719 + 3) % 32749;
	return ((a % lim) + 1);
}
