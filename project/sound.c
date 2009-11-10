/*
 * FILE: sound.c
 *
 * Written by Justin Mancinelli
 */

#include <avr/io.h>
#include "timer.h"
#include "terminalio.h"
#include <avr/pgmspace.h>
#include <stdio.h>


int8_t soundStatus = 1;

void init_sound(void){
	DDRD = (1<<DDD5) | (1<<DDD4);
	PORTD = 0;
	TCCR1A = 0x40;
	OCR1A = 1999;
	TCCR1B = 0x08;
}

void play_sound(void){
	TCCR1B |= 0x01;
	wait_for(300);
	TCCR1B &= 0xFE;
}

void toggle_sound(void){
	soundStatus = !soundStatus;
}

int8_t sound_status(void){
	return soundStatus;
}

void display_sound_status(void){
	move_cursor(1,3);
	if(soundStatus){
		printf_P(PSTR("Sound: On "));
	}
	else{
		printf_P(PSTR("Sound: Off"));
	}
}
