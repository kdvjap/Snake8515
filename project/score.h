/*
** score.h
** 
** Written by Peter Sutton
*/

/* Guard band to ensure this definition is only included once */
#ifndef SCORE_H
#define SCORE_H

#include <inttypes.h>

void init_score(void);
void add_to_score(uint16_t value);
uint16_t get_score(void);

#endif
