/*
 * FILE: savestate.c
 *
 * Handle saving game state to EEPROM
 *
 * Written by Justin Mancinelli
 */

#include "savestate.h"

/*for debugging
#include "led_display.h"
#include "terminalio.h"
#include <stdio.h>
#include <avr/pgmspace.h>
*/

uint16_t EEMEM ee_state_validation;

/* EEPROM variables */
//snake variables
PosnType EEMEM ee_snakePositions[MAX_SNAKE_SIZE];
int8_t EEMEM ee_snakeHeadIndex;
int8_t EEMEM ee_snakeTailIndex;
int8_t EEMEM ee_curSnakeDirn;
int8_t EEMEM ee_nextSnakeDirn;

//food variables
PosnType EEMEM ee_foodPositions[MAX_FOOD];
int8_t EEMEM ee_numFoodItems;

//score variables
uint16_t EEMEM ee_score;

//timer variables
volatile uint16_t EEMEM ee_sw_timer_values[NUM_SW_TIMERS+1];
volatile uint16_t EEMEM ee_sw_timer_target[NUM_SW_TIMERS+1];
TimerFunctionType* EEMEM ee_sw_timer_functions[NUM_SW_TIMERS+1];
volatile uint8_t EEMEM ee_sw_timer_once_only[NUM_SW_TIMERS+1];

/* external variables */
//snake variables
extern PosnType snakePositions[MAX_SNAKE_SIZE];
extern int8_t snakeHeadIndex;
extern int8_t snakeTailIndex;
extern int8_t curSnakeDirn;
extern int8_t nextSnakeDirn;

//food variables
extern PosnType foodPositions[MAX_FOOD];
extern int8_t numFoodItems;

//score variables
extern uint16_t score;

//clock variables
extern uint16_t sw_timer_values[NUM_SW_TIMERS+1];
extern uint16_t sw_timer_target[NUM_SW_TIMERS+1];
extern TimerFunctionType* sw_timer_functions[NUM_SW_TIMERS+1];
extern uint8_t sw_timer_once_only[NUM_SW_TIMERS+1];

void save_state(void){
	if(eeprom_read_word(&ee_state_validation) != 31415)
		eeprom_write_word(&ee_state_validation, 31415);

	/* Recall: typedef uint8_t PosnType;
	**	       eeprom_write_block(const void *  __src, void *  __dst, size_t  __n)
	** 		   eeprom_write_word(uint16_t * __p, value);
	*/

	//snake variables
	eeprom_write_block((const void*)&snakePositions, (void*)&ee_snakePositions, MAX_SNAKE_SIZE);
	eeprom_write_byte((uint8_t *)&ee_snakeHeadIndex, snakeHeadIndex);
	eeprom_write_byte((uint8_t *)&ee_snakeTailIndex, snakeTailIndex);
	eeprom_write_byte((uint8_t *)&ee_curSnakeDirn, curSnakeDirn);
	eeprom_write_byte((uint8_t *)&ee_nextSnakeDirn, nextSnakeDirn);

	//food variables
	eeprom_write_block((const void*)&foodPositions, (void*)&ee_foodPositions, MAX_FOOD);
	eeprom_write_byte((uint8_t *)&ee_numFoodItems, numFoodItems);

	//score variables
	eeprom_write_word(&ee_score, score);

	//timer variables
	eeprom_write_block((const void*)&sw_timer_values, (void*)&ee_sw_timer_values, NUM_SW_TIMERS+1);
	eeprom_write_block((const void*)&sw_timer_target, (void*)&ee_sw_timer_target, NUM_SW_TIMERS+1);
	eeprom_write_block((const void*)&sw_timer_functions, (void*)&ee_sw_timer_functions, NUM_SW_TIMERS+1);
	eeprom_write_block((const void*)&sw_timer_once_only, (void*)&ee_sw_timer_once_only, NUM_SW_TIMERS+1);

	/*for debugging
	move_cursor(1,20);
	printf_P(PSTR("Snake Positions:"));
	int8_t i;
	for(i = 0; i < MAX_SNAKE_SIZE; i++)	
		printf_P(PSTR(" %u"), snakePositions[i] );
	*/
}

int8_t load_state(void){
	if(eeprom_read_word(&ee_state_validation) == 31415){
		/* Recall: typedef uint8_t PosnType;
		** 		   eeprom_read_block( void *  __dst, const void *  __src, size_t  __n    )
		**		   uint16_t eeprom_read_word( const uint16_t *__p) 
		*/

		//snake variables
		eeprom_read_block((void*)&snakePositions, (const void*)&ee_snakePositions, MAX_SNAKE_SIZE);
		snakeHeadIndex = (int8_t)eeprom_read_byte((const uint8_t *)&ee_snakeHeadIndex);
		snakeTailIndex = (int8_t)eeprom_read_byte((const uint8_t *)&ee_snakeTailIndex);
		curSnakeDirn = (int8_t)eeprom_read_byte((const uint8_t *)&ee_curSnakeDirn);
		nextSnakeDirn = (int8_t)eeprom_read_byte((const uint8_t *)&ee_nextSnakeDirn);

		//food variables
		eeprom_read_block((void*)&foodPositions, (const void*)&ee_foodPositions, MAX_FOOD);
		numFoodItems = (int8_t)eeprom_read_byte((const uint8_t *)&ee_numFoodItems);

		//score variables
		score = eeprom_read_word((const uint16_t *)&ee_score);

		//timer variables
		eeprom_read_block((void*)&sw_timer_values, (const void*)&ee_sw_timer_values, NUM_SW_TIMERS+1);
		eeprom_read_block((void*)&sw_timer_target, (const void*)&ee_sw_timer_target, NUM_SW_TIMERS+1);
		eeprom_read_block((void*)&sw_timer_functions, (const void*)&ee_sw_timer_functions, NUM_SW_TIMERS+1);
		eeprom_read_block((void*)&sw_timer_once_only, (const void*)&ee_sw_timer_once_only, NUM_SW_TIMERS+1);

		/*for debugging
		move_cursor(1,20);
		printf_P(PSTR("Snake Positions:"));
		int8_t i;
		for(i = 0; i < MAX_SNAKE_SIZE; i++)	
			printf_P(PSTR(" %u"), snakePositions[i] );
		*/
		return 1;
	} else
		return 0;
}
