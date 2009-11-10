/*
 * FILE: savestate.h
 *
 * Handle saving game state to EEPROM
 *
 * Written by Justin Mancinelli
 */

#include <stdint.h>
#include <avr/eeprom.h>

//these hold datastructures we need to copy
#include "snake.h"
#include "food.h"
#include "wall.h"
#include "timer.h"
#include "score.h"


void save_state(void);
int8_t load_state(void);
