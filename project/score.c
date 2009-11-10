/*
** score.c
**
** Written by Peter Sutton
*/

#include <stdint.h>
#include <avr/eeprom.h>
#include "terminalio.h"
#include <stdio.h>
#include <avr/pgmspace.h>

uint16_t score;	/* Can represent values from 0 to 65535 */

//4209435
uint16_t EEMEM ee_highscore;
uint16_t EEMEM ee_score_validation;
uint16_t highscore; /* this will save the current highscore in program memory */

void init_score(void) {
	score = 0;

	if(eeprom_read_word(&ee_score_validation) != 31415){
		eeprom_write_word(&ee_score_validation, 31415);
		eeprom_write_word(&ee_highscore, 0);
		highscore = 0;
	}
	else
		highscore = eeprom_read_word(&ee_highscore);

}


void add_to_score(uint16_t value) {
	score += value;

	//4209435
	if(highscore <= score){
		eeprom_write_word(&ee_highscore, score);
		highscore = score;
	}
}

uint16_t get_score(void) {
	return score;
}

//4209435
uint16_t get_highscore() {
	return highscore;
}

void update_score(void){
	move_cursor(1,1);
	printf_P(PSTR("Score: %u\n"), get_score());
	printf_P(PSTR("High Score: %u"), get_highscore());

/*	move_cursor(8,1);
	clear_to_end_of_line();
	printf_P(PSTR("%u"), get_score());

	move_cursor(13,2);
	clear_to_end_of_line();
	printf_P(PSTR("%u"), get_highscore()); */

	/* Debug *
	move_cursor(0, TITLEY);
	printf_P(PSTR("update_score %u"), get_score());
	wait_for(1000);
	//*/
}
