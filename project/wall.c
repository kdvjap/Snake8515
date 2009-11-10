/*
** wall.c
**
** Written by Justin Mancinelli
**
** Handle stationary obstacles
*/

#include "wall.h"
#include "board.h"
#include "timer.h"

//for debugging
#include "led_display.h"
#include "terminalio.h"
#include <stdio.h>
#include <avr/pgmspace.h>
//


/* Store the wall positions created by a tail-cut in an array */
PosnType wallPositions[MAX_WALL_SIZE];

/* Store wall indexes 
** Store as position type, but
** x value is the start index
** y value is the end index
*/
PosnType wallIndexes[MAX_NUM_WALLS];

// Keep track of wall segments
uint8_t wallInsertionIndex;
// Keep track of entire walls
uint8_t newWallIndex;

void init_walls(void){
	wallInsertionIndex = 0;
	while(wallInsertionIndex < MAX_WALL_SIZE){
		wallPositions[wallInsertionIndex++] = 0xFF;
	}
	wallInsertionIndex = 0;
}

/* is_wall_at
**		Check all wall positions and see if any part of the 
**		wall is at the given position
*/
int8_t is_wall_at(PosnType position) {
	int8_t index = 0;

	while(index < MAX_WALL_SIZE) {
		if(position == wallPositions[index]) {
			return 1;
		}
		index++;
	}

	/* There is no wall here */
	return 0;
}

/* Adds a wall element at the given position */
int8_t add_wall_at(PosnType position){
	//Don't add a wall if there's no more room
	if(wallInsertionIndex < MAX_WALL_SIZE){
		wallPositions[wallInsertionIndex++] = position;
		return 1;
	}
	//we couldn't place a wall
	return 0;
}

void show_walls(void) {
	int8_t i;
	for(i=0; i < MAX_WALL_SIZE; i++) {
	//food and walls are the same in terms of lighting
		add_food_item_to_board(wallPositions[i]);
	}
}

/* inserts a wall into wallIndexes and flags it for
** removal after a certain amount of time
*/
void flag_wall(PosnType wall){
	if(newWallIndex < MAX_NUM_WALLS) {
		wallIndexes[newWallIndex++] = wall;
		execute_function_once_after_delay(10000, remove_wall);
	}
}

/* removes a wall from wallIndexes and removes it
** from the board. Note, they will be removed in the
** order they were added
*/
void remove_wall(){
	int8_t i, j, start, stop;
	start = x_position(wallIndexes[0]);
	stop = y_position(wallIndexes[0]);
	//shift wallPositions to create more space at the end
	//remove from board as we go along
	for(i = start; i < stop; i++) {
		remove_food_item_from_board(wallPositions[start]);
		
		//empty_display();
		for(j = start; j < MAX_WALL_SIZE; j++){
			wallPositions[j] = wallPositions[j + 1];
		}
	}
	wallInsertionIndex -= stop - start;

	//shift wallIndexes
	for(i = 0; i < MAX_NUM_WALLS; i++){
		wallIndexes[i] = wallIndexes[i + 1];
	}
	newWallIndex--;
}
