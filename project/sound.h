/*
 * FILE: sound.h
 *
 * Written by Justin Mancinelli
 */

 
/* Guard band to ensure this definition is only included once */
#ifndef SOUND_H
#define SSOUND_H

void init_sound(void);

void play_sound(void);

void toggle_sound(void);

int8_t sound_status(void);

void display_sound_status(void);


#endif
