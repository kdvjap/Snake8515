/*
** snake.h
**
** Written by Peter Sutton
**
** Details of the snake
*/

/* Guard band to ensure this definition is only included once */
#ifndef SNAKE_H
#define SNAKE_H

#include <inttypes.h>
#include "position.h"

#define MAX_SNAKE_SIZE 32

/* Directions */
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/* Possible results of an attempt to move the snake */
#define OUT_OF_BOUNDS -1
#define COLLISION -2
#define MOVE_OK 1
#define ATE_FOOD 2

/* init_snake()
**
** Initialise the snake. 
*/
void init_snake(void);

/* get_snake_head_position()
**
** Returns the position of the head of the snake. 
** (Should only be called after the snake is initialised.)
*/
PosnType get_snake_head_position(void);

/* get_snake_length()
**
** Returns the current length of the snake.
** (Must be between 2 and MAX_SNAKE_SIZE inclusive.)
** (Should only be called after the snake is initialised.)
*/
int8_t get_snake_length(void);

/* move_snake()
**
** Attempt to move the snake by one in the current direction.
** Returns -1 if the snake has run into the edge or itself,
** 0 if the move is successful, 1 if the snake has eaten
** some food (and grown - if possible.). 
*/
int8_t move_snake(void);

/* set_snake_dirn(direction)
**
** Attempt to set snake direction to one of UP, DOWN, 
** LEFT or RIGHT. Returns 1 if successful, 0 otherwise.
** (Will fail if try and reverse snake - OK to continue
** in same direction or turn 90degrees.) The direction
** will take effect when move_snake() is called. This
** function can be called multiple times - only the last
** value set before move_snake() is called will be used.
*/
int8_t set_snake_dirn(int8_t dirn);

/* is_snake_at(position)
**
** Returns 1 if the given position is occupied by 
** some part of the snake, 0 otherwise.
*/
int8_t is_snake_at(PosnType position);

#endif
