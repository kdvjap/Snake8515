/*
** snake.c
**
** Written by Peter Sutton
**
** Details of the snake and making it move.
*/

#include "position.h" //17 Oct
#include "snake.h"
#include "board.h"
#include "food.h"
#include "wall.h"

//for debugging
#include "led_display.h"
#include "terminalio.h"
#include <stdio.h>
#include <avr/pgmspace.h>
#include "timer.h"
//

/* Global variables */
/* We store the snake in a circular buffer - an array which can 
** wrap around from the end to the start. The array is of size
** MAX_SNAKE_SIZE, which is the maximum length of the snake.
*/
PosnType snakePositions[MAX_SNAKE_SIZE];

/* Index values in the array of the head and tail positions of the snake.
** The head position is to the right of the tail position (until the head
** wraps around).
** (The length of the snake is (head index - tail index + 1) and if
** this result is 0 or negative then add 32.) 
** (The index values are in the range of 0 to MAX_SNAKE_SIZE-1 inclusive.)
*/
int8_t snakeHeadIndex;
int8_t snakeTailIndex;

/*
** Variable to keep track of the current direction 
** of the snake and the next direction of the
** snake. (The current direction will become the
** next direction when we move the snake.) The 
** directions must be one of LEFT, RIGHT, 
** UP or DOWN. (We keep track of both of these
** because (1) we allow directions to be changed
** multiple times between moves (and only the last
** one will count) and (2) we can never allow the
** snake to go back on itself, it's only possible
** to turn 90degrees or keep going in the same direction.
*/
int8_t curSnakeDirn;
int8_t nextSnakeDirn;


/* FUNCTIONS */
/* init_snake()
**
** Resets out snake to the initial configuration
*/
void init_snake(void) {
	/* Snake starts at (0,0) and finishes at (0,2) and
	** has an initial length of 3. These positions will
	** be stored at indexes 0, 1 and 2 in the array.
	*/
	snakeTailIndex = 0;
	snakeHeadIndex = 2;
	snakePositions[0] = 0x00;
	snakePositions[1] = 0x01;
	snakePositions[2] = 0x02;
	curSnakeDirn = UP;
    nextSnakeDirn = UP;
	add_snake_element_to_board(0x00);
	add_snake_element_to_board(0x01);
	add_snake_element_to_board(0x02);
}

/* get_snake_head_position()
**
** Returns the position of the head of the snake. 
*/
PosnType get_snake_head_position(void) {
    return snakePositions[snakeHeadIndex];
}

/* get_snake_length()
**
** Returns the length of the snake.
*/
int8_t get_snake_length(void) {
	int8_t len;
	/* (The length of the snake is (head index - tail index + 1) and if
	** this result is 0 or negative then add 32.) 
	*/
	len = snakeHeadIndex - snakeTailIndex + 1;
	if (len <= 0) {
		len += MAX_SNAKE_SIZE; //17Oct
	}
	return len;
}

/* 
** move_snake() 
**
**      Attempt to move the snake by one in the new direction.
**      Returns -1 (OUT_OF_BOUNDS) if the snake has run into the 
**		edge, -2 (COLLISION) if the snake has run into itself,
**      1 (MOVE_OK) if the move is successful, 2 (ATE_FOOD)
**		if the snake has eaten
**      some food (and grown). (The snake will only grow if
** 		there is room for it to do so in the array.).
*/
int8_t move_snake(void) {
    int8_t foodAtHead;	/* True if food at new head position */
	int8_t headX;	/* head X position */
	int8_t headY;	/* head Y position */
	PosnType headPosn;
    
	/* Current head position */
	headX = (snakePositions[snakeHeadIndex] >> 4) & 0x07;
	headY = snakePositions[snakeHeadIndex] & 0x0F;
    
    /* Work out where the new head position should be - we
    ** move 1 position in our NEXT direction of movement.
    */
    switch (nextSnakeDirn) {
        case UP:
            headY += 1;
            break;
        case RIGHT:
            headX += 1;
            break;
        case DOWN:
            headY -= 1;
            break;
        case LEFT:
            headX -= 1;
            break;
    }

	headPosn = position(headX, headY);

	/* Update the current direction */
	curSnakeDirn = nextSnakeDirn;

	/* ADD CODE HERE to check whether the new head position
	** is off the board, and if so return OUT_OF_BOUNDS. Do
	** not continue. See board.h for a function which can help you.
	*/
	
	if(is_off_board((get_snake_head_position() >> 4) & 0x07, 
				    get_snake_head_position() & 0x0F))
		return OUT_OF_BOUNDS;


	/* ADD CODE HERE to check whether the new head position
	** is already occupied by the snake, and if so, return
	** COLLISION. Do not continue. See snake.h for a function 
	** which can help you.
	*/
	/*
	if(is_snake_at(headPosn))
		return COLLISION;
	//*/

	//4209435
	/* Modification of above to impliment Tail-Cut
	** Instead of returning COLLISION, cut the tail at the
	** point of collision and save it as a wall
	*/
	int8_t c_index;
	c_index = is_snake_at(headPosn);
	if(c_index){
		uint8_t start;
		uint8_t stop;
		extern int8_t wallInsertionIndex;
		//store the start of the wall
		start = wallInsertionIndex;

		//copy from snake to collision point to wall array
		//while trimming the snake along the way
		while(snakeTailIndex < c_index){
			add_wall_at(snakePositions[snakeTailIndex++]);
		}
		//store the end of the wall
		stop = wallInsertionIndex - start;
		start = 0;
		
		//we just built a wall so flag it for deletion
		flag_wall(position(start, stop));

	}
	//*/


	//4209435
	/* Check if there is a wall at the head position
	** If there is, return COLLISION
	** This helps implement Tail-Cut functionality
	*/
	if(is_wall_at(headPosn))
		return COLLISION;



    /* Check if there is food at the head position
    ** Value will be -1 if no food, otherwise value will
    ** be the food ID
    */
    foodAtHead = food_at(headPosn);
    
    /*
    ** If we get here, the move should be possible.
    ** Advance head by 1. First work out the index
	** of the new head position in the array.
    */
	snakeHeadIndex++;
	if(snakeHeadIndex == MAX_SNAKE_SIZE) {
		/* Array has wrapped around */
		snakeHeadIndex = 0;
	}
	
	/* Advance the tail position. Note that we've already
	** updated the head index - but not stored the new head
	** position there yet since if the snake is maximum size
	** the new head position will go into the array where the 
	** old tail position was.
	*/ 
	/* Remove tail position from the board */
	remove_snake_element_from_board(snakePositions[snakeTailIndex]);
	/* Update the tail index */
	snakeTailIndex++;
	if(snakeTailIndex == MAX_SNAKE_SIZE) {
		/* Array has wrapped around */
		snakeTailIndex = 0;
	}

	/* Store the head position and display it */
	snakePositions[snakeHeadIndex] = headPosn;
	add_snake_element_to_board(snakePositions[snakeHeadIndex]);

	/* YOUR CODE HERE to (1) if the snake ate food and if so, to remove the 
	** food, add a new item of food and return ATE_FOOD.
	*/
	if(foodAtHead != -1){
		/*for debugging *
		move_cursor(1, 15);
		printf_P(PSTR("foodID: %u"), foodAtHead );
		wait_for(1000);
		//*/
		remove_food(foodAtHead);

		//increase the length of the snake unless snake is 40 segments
		if(get_snake_length() < 40)
			--snakeTailIndex;

		return ATE_FOOD;
	}

	/* Otherwise we return MOVE_OK, indicating that the move was
	** successful.
	*/
    return MOVE_OK;
}

/* set_snake_dirn
**      Attempt to set the next snake direction to one of 
**      UP, DOWN, LEFT or RIGHT. Returns 1 if successful, 
**      0 otherwise.
**      (Will fail if try and reverse snake from its current
**      direction, but otherwise the direction will be accepted.)
**      It's OK to continue in same direction or turn 90degrees.
*/
int8_t set_snake_dirn(int8_t dirn) {
	/* YOUR CODE HERE - MODIFY THIS FUNCTION */
	/* You must write code to check that the proposed direction (dirn)
	** is not opposite to the current direction (stored in curSnakeDirn). 
	** If the direction is OK, set the next direction (stored in nextSnakeDirn) 
	** and return 1, otherwise return 0.
	**
	** Initially, we assume the move is OK and just set the 
	** next direction.
	*/
    switch (dirn) {
    case UP:
        if(curSnakeDirn != DOWN)
		 	nextSnakeDirn = dirn;
		else
			return 0;
    case RIGHT:
        if(curSnakeDirn != LEFT)
			nextSnakeDirn = dirn;
		else
			return 0;
    case DOWN:
        if(curSnakeDirn != UP)
			nextSnakeDirn = dirn;
		else
			return 0;
    case LEFT:
        if(curSnakeDirn != RIGHT)
			nextSnakeDirn = dirn;
		else
			return 0;
    }
    return 1;

}

/* is_snake_at
**		Check all snake positions and see if any part of the 
**		snake is at the given position
*/

int8_t is_snake_at(PosnType position) {
	int8_t index;

	/* Start at tail and work forward to the head.
	*/
	index = snakeTailIndex;
	while(index != snakeHeadIndex) {
		if(position == snakePositions[index]) {
			//return 1;
			//4209435
			return index;
		}
		index++;
		if(index == MAX_SNAKE_SIZE) { //19 Oct
			index = 0;
		} 
	}
	/* Now check head position, since it is not checked above. */
	if(position == snakePositions[snakeHeadIndex]) {
		return 1;
	}
	/* Snake does not occupy the given position */
	return 0;
}

//4209435
void show_snake(void) {
	int8_t i;

	//hack to fix a jump by one after pausing
	snakeHeadIndex--;
	snakeTailIndex--;

	for(i=0; i < MAX_SNAKE_SIZE; i++) {

		//only do it for visible elements
		//snake hasn't wrapped around
		if(snakeHeadIndex > snakeTailIndex) {
			if( i <= snakeHeadIndex && i >= snakeTailIndex)
				add_snake_element_to_board(snakePositions[i]);
		}
		//snake has wrapped around
		else {
			if( i <= snakeHeadIndex || i >= snakeTailIndex)
				add_snake_element_to_board(snakePositions[i]);
		}
	}
}
