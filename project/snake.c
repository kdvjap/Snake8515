/*
** snake.c
**
** Written by Peter Sutton
**
** Details of the snake and making it move.
*/

#include "snake.h"
#include "board.h"
#include "food.h"

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
	snakePositions[0] = position(0,0);
	snakePositions[1] = position(0,1);
	snakePositions[2] = position(0,2);
	curSnakeDirn = UP;
    nextSnakeDirn = UP;
	add_snake_element_to_board(position(0,0));
	add_snake_element_to_board(position(0,1));
	add_snake_element_to_board(position(0,2));
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
		len += 32;
	}
	return len;
}

/* 
** move_snake() 
**
**      Attempt to move the snake by one in the new direction.
**      Returns -1 (OUT_OF_BOUNDS) if the snake has run into the 
**		edge, -2 (COLLISION) if the snake has run into itself,
**      0 (MOVE_OK) if the move is successful, 1 (ATE_FOOD)
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
	headX = x_position(snakePositions[snakeHeadIndex]);
	headY = y_position(snakePositions[snakeHeadIndex]);
    
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
		/* YOUR CODE HERE to deal with other directions */
    }

	headPosn = position(headX, headY);

	/* Update the current direction */
	curSnakeDirn = nextSnakeDirn;

	/* ADD CODE HERE to check whether the new head position
	** is off the board, and if so return OUT_OF_BOUNDS. Do
	** not continue. See board.h for a function which can help you.
	*/    
   


	/* ADD CODE HERE to check whether the new head position
	** is already occupied by the snake, and if so, return
	** COLLISION. Do not continue. See snake.h for a function 
	** which can help you.
	*/



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
    
    nextSnakeDirn = dirn;
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
			return 1;
		}
		index++;
		if(index > MAX_SNAKE_SIZE) {
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
		
