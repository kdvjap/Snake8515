/*
** score.c
**
** Written by Peter Sutton
*/

#include <stdint.h>

uint16_t score;	/* Can represent values from 0 to 65535 */

void init_score(void) {
	score = 0;
}

void add_to_score(uint16_t value) {
	score += value;
}

uint16_t get_score(void) {
	return score;
}
