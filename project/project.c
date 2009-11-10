/*
 * FILE: project.c
 *
 * This is the main file.
 *
 * Written by Peter Sutton
 */

#include "board.h"
#include "snake.h"
#include "led_display.h"
#include "score.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//42094353
#include "sound.h"
#include "food.h"
#include "wall.h"
#include "savestate.h"

#define SPLASHBOXSTARTY 6
#define SPLASHBOXENDY	16
#define SPLASHBOXSTARTX 20
#define SPLASHBOXENDX	60
#define TITLEX			38
#define TITLEY			8
#define NAMEX			22
#define NAMEY			11
#define IDX				22
#define IDY				13
#define INSTRUCTY		18
#define NEWGAME			0
#define PAUSE			2
#define	GAMEOVER		-1
#define PLAYING			1
#define BLINKRATE		100
#define RATSPEED		921
 

/*
** Function prototypes - these are defined below main()
*/
void new_game(void);
void splash_screen(void);
void handle_game_over(void);
void time_increment(void);
//4209435
void show_instruction(int8_t);
void update_score(void);
void pause_game(void);

volatile uint8_t timePassedFlag = 0;
int8_t mainTimerNum;
//4209435
int8_t foodTimerNum;
int8_t ratsTimerNum;

/*
 * main -- Main program.
 */
int main(void) {
	uint8_t chars_into_escape_sequence = 0;
	int8_t moveStatus = 0;

	char c;

	/* Initialise our main clock */
	init_timer();

	/* Initialise serial I/O */
	init_serial_stdio(19200, 0);

	/* Make the display_row() function be called every 2ms.
	** (This function returns a timer number, but we ignore 
	** this since we'll never do anything with it.)
	*/
	execute_function_periodically(2, display_row);

	/* Register the time_increment() function to be called every 500ms.
	** This function just sets a variable (timePassedFlag).
	*/
	mainTimerNum = execute_function_periodically(500, time_increment);

	//4209435
	/* setup AVR to handle sounds*/
	init_sound();

	/*
	** Turn on interrupts (needed for timer and serial input/output to work)
	*/
	sei();
	
	/*
	** Display splash screen 
	*/
	splash_screen();
	show_instruction(NEWGAME);
	
	/*
	** Perform necessary initialisations for a new game.
	*/
	new_game();
		
	/*
	** Event loop - wait for a certain amount of time to pass or wait
	** for a character to arrive from standard input. The time_passed_flag
	** is set within the function time_increment() below - which is setup
	** to be called periodically.
	*/
	for(;;) {
		if(timePassedFlag) {
			moveStatus = move_snake();
			timePassedFlag = 0;
		} else if(input_available()) {
			/* Read the input from our terminal and handle it */
			c = fgetc(stdin);			
			if(chars_into_escape_sequence == 0 && c == '\x1b') {
				/*
				** Received ESCAPE character - we're one character into
				** an escape sequence
				*/
				chars_into_escape_sequence = 1;
			} else if(chars_into_escape_sequence == 1 && c == '[') {
				/* 
				** We're now two characters into an escape sequence
				*/
				chars_into_escape_sequence = 2;
			} else if (chars_into_escape_sequence == 2) {
				/* We're two characters into an escape sequence and
				** have received another - see if it is as expected.
				*/
				if (c == 'C') {
					/* Cursor right key pressed - Set next direction to
					** be moved to RIGHT */
					set_snake_dirn(RIGHT);
				}  
				if (c == 'D') {
					/* Cursor left key pressed - Set next direction to
					** be moved to LEFT */
					set_snake_dirn(LEFT);
				}  
				if (c == 'A') {
					/* Cursor up key pressed - Set next direction to
					** be moved to UP */
					set_snake_dirn(UP);
				}  
				if (c == 'B') {
					/* Cursor down key pressed - Set next direction to
					** be moved to DOWN */
					set_snake_dirn(DOWN);
				}

				/* else, unknown escape sequence */

				/* We're no longer part way through an escape sequence */
				chars_into_escape_sequence = 0; 
			} else if(chars_into_escape_sequence != 0) {
				/*
				** We started an escape sequence but didn't get a character
				** we recognised - discard it and assume that we're not
				** in an escape sequence.
				*/
				chars_into_escape_sequence = 0;
			} else if (c == ' ') {
				/* Space character received - move snake immediately */
				moveStatus = move_snake();
			} else {					
				if(c == 'N' || c == 'n'){	
					show_instruction(NEWGAME);				
					new_game();
				} else if(c == 'P' || c == 'p'){
					moveStatus = 0;
					pause_game();
				} else if(c == 'M' || c == 'm'){
					toggle_sound();
					display_sound_status();
				}
			}
		}

		switch(moveStatus){
			case ATE_FOOD:
				if(sound_status())
					play_sound();
				moveStatus = MOVE_OK;
				break;
		}

		if(moveStatus < 0) {
			/* Move failed - game over */
			handle_game_over();
			moveStatus = 0;
			update_score();
		}
	}
}

void time_increment(void) {
	/* Function that gets called when a certain amount of time passes */
	timePassedFlag = 1;
}

void new_game(void) {
	char c = 0;
	cancel_software_timer(foodTimerNum);
	cancel_software_timer(ratsTimerNum);
	empty_display();

	//wait for space
	while(c != ' ' && c != 'l' && c != 'L'){
		if(input_available())
			c = fgetc(stdin);

		if(c == 'M' || c == 'm'){
			toggle_sound();
			display_sound_status();
			c = 0;
		}
	}
	
	init_display();
	
	if(c == 'l' || c == 'L'){
		if(load_state())
			render_board();
		else {
			/* Initialise internal representations. */
			init_board();
			init_score();
		}
	}
	else {
		/* Initialise internal representations. */
		init_board();
		init_score();
	}
	clear_terminal();

	//Place scores
	update_score();

	//place sound status
	display_sound_status();

	show_instruction(PLAYING);

	/* Make food blink 5 times a second. We should call blink_food
	** 10 times a second which is 100ms
	*/
	foodTimerNum = execute_function_periodically(BLINKRATE, blink_food);
	ratsTimerNum = execute_function_periodically(RATSPEED, move_rats);

	/* Debug *
	move_cursor(0, TITLEY-1);
	printf_P(PSTR("new_game %u"), get_score());
	wait_for(1000);
	//*/
}

void splash_screen(void) {

	/* Clear the terminal screen */
	clear_terminal();
	init_score();
	update_score();

	/* 
	** Display some suitable message to the user that includes your name(s)
	** and student number(s).
	** You may need to use terminalio functions to position the cursor 
	** appropriately - see terminalio.h for details.
	*/
	display_sound_status();

	move_cursor(0,TITLEY);
	printf_P(PSTR("Snake\n\nBy: Justin Mancinelli\n\nID: 42094353"));
	//move_cursor(NAMEX,NAMEY);
	//printf_P(PSTR(""));
	//move_cursor(IDX,IDY);
	//printf_P(PSTR(""));

}

void show_instruction(int8_t status){
	move_cursor(1, INSTRUCTY + 1);
	clear_to_end_of_line();
	move_cursor(1, INSTRUCTY);
	clear_to_end_of_line();
	
	switch(status){
		case NEWGAME:	
			printf_P(PSTR("Welcome! Press 'space' to start a new game.\nPress 'L' to to load a saved game."));
			//printf_P(PSTR("Press 'L' to to load a saved game."));
			break;
		case GAMEOVER:
			printf_P(PSTR("Game Over! Press 'space' to start a new game.\nPress 'L' to to load a saved game."));
			//printf_P(PSTR("Press 'L' to to load a saved game."));
			break;
		case PAUSE:
			printf_P(PSTR("Paused... Press 'P' to continue.\nPress 'S' to save game state."));
			//printf_P(PSTR("Press 'S' to save game state."));
			break;
		case PLAYING:
			move_cursor(1, TITLEY);
			clear_to_end_of_line();
			move_cursor(1, INSTRUCTY);
			printf_P(PSTR("Have fun! Press 'N' to start a new game.\nPress 'P' to to pause the game."));
			//printf_P(PSTR("Press 'P' to to pause the game."));
			break;
	}			
}

void handle_game_over(void) {
	cancel_software_timer(foodTimerNum);
	cancel_software_timer(ratsTimerNum);
	splash_screen();	
	show_instruction(GAMEOVER);
	new_game();
}

void pause_game() {
	/*
	** status = 0, game is running
	** status = 1, game is paused
	*/
	static uint8_t status = 0;
	char c = 0;

	if(status) {
		move_cursor(28, TITLEY);
		clear_to_end_of_line();
		show_instruction(PLAYING);
		render_board();
		foodTimerNum = execute_function_periodically(BLINKRATE, blink_food);
		ratsTimerNum = execute_function_periodically(RATSPEED, move_rats);
		status = 0;
	}
	else {
		show_instruction(PAUSE);
		cancel_software_timer(foodTimerNum);
		cancel_software_timer(ratsTimerNum);
		empty_display();
		status = 1;

		while(c != 'p' && c != 'P'){
			if(input_available())
				c = fgetc(stdin);

			if(c == 's' || c == 'S'){
				save_state();
				//move_cursor(0, TITLEY);
				//clear_to_end_of_line();
				move_cursor(28, TITLEY);
				printf_P(PSTR("State has been saved."));
				c = 0;
			}

		}
		pause_game();

	}
}
