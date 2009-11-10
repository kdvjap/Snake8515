/*
** wall.h
**
** Written by Justin Mancinelli
**
** Handle stationary obstacles
*/

#include <inttypes.h>
#include "position.h"

#define MAX_WALL_SIZE 36

/* Store start and stop indexes of walls
** Each wall is at least one segment long
** Choose worst case of all walls one segment
*/
#define MAX_NUM_WALLS 36 

void init_walls(void);

/* is_wall_at
**		Check all wall positions and see if any part of the 
**		wall is at the given position
*/
int8_t is_wall_at(PosnType);

/* Adds a wall element at the given position */
int8_t add_wall_at(PosnType);

/* show_walls(void)
**
** Display the walls on the board
*/
void show_walls(void);

/* flag_wall(void)
**
** Inserts a new wall and flags it for deletion
*/
void flag_wall(PosnType);

/* remove_wall(void)
**
** Removes the first wall in the list
*/
void remove_wall(void);
