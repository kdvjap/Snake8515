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

/*
** Function prototypes - these are defined below main()
*/
void new_game(void);
void splash_screen(void);
void handle_game_over(void);
void time_increment(void);

volatile uint8_t timePassedFlag = 0;
int8_t mainTimerNum;

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

	/*
	** Turn on interrupts (needed for timer and serial input/output to work)
	*/
	sei();
	
	/*
	** Display splash screen 
	*/
	splash_screen();
	
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
				/* YOUR CODE HERE TO DEAL WITH OTHER DIRECTIONS */
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
				/*
				** Some other character received. Handle it (or ignore it).
				*/
								
				/*
				** YOUR CODE HERE TO CHECK FOR OTHER KEY PRESSES
				** AND TAKE APPROPRIATE ACTION. You may need to 
				** add code in other locations also.
				*/
			}
		}
		if(moveStatus < 0) {
			/* Move failed - game over */
			handle_game_over();
			moveStatus = 0;
		}
	}
}

void time_increment(void) {
	/* Function that gets called when a certain amount of time passes */
	timePassedFlag = 1;
}

void new_game(void) {
	/* 
	** Initialise the LED display and internal representations.
	*/
	init_display();
	init_board();
	init_score();
}

void splash_screen(void) {

	/* Clear the terminal screen */
	clear_terminal();
	
	/* YOUR CODE HERE - replace/modify the following */
	printf_P(PSTR("Snake\n"));

	/* 
	** Display some suitable message to the user that includes your name(s)
	** and student number(s).
	** You may need to use terminalio functions to position the cursor 
	** appropriately - see terminalio.h for details.
	*/

	/* Add a wait for a space bar to be pressed. See the code above for how
	** input characters can be dealt with.
	*/
}

/* Initially, this function is not called from anywhere */
void handle_game_over(void) {
	/* Print "Game over" to the terminal */
	printf_P(PSTR("Game over")); 

	/* ADD/MODIFY CODE BELOW to start a new game and show the splash screen 
	** again.
	*/
	/* The code below turns off our time_increment timer - which stops 
	** the snake moving (on a timed basis). You will need to remove this
	** code. */
	cancel_software_timer(mainTimerNum);
}
