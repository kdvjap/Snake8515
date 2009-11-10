/*
** timer.c
**
** Written by Peter Sutton
*/

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Our global timer variable - counts in milliseconds. Will wrap 
** around after 65535
*/
volatile uint16_t time;

/* Software timer values, duration targets, functions to be
** executed when the target is reached and flags to
** indicate whether we do this once or repeatedly If the target
** duration is non-zero, then the timer is active. (The
** values, target and once only flag can be changed in 
** the interrupt service routine so are labeled volatile.)
*/
volatile uint16_t sw_timer_values[NUM_SW_TIMERS+1];
volatile uint16_t sw_timer_target[NUM_SW_TIMERS+1];
TimerFunctionType* sw_timer_functions[NUM_SW_TIMERS+1];
volatile uint8_t sw_timer_once_only[NUM_SW_TIMERS+1];


/* Set up AVR timer/counter 0 to give an interrupt 
** 500 times per second (once every 2 milliseconds). We will divide 
** the system clock by 64 so that timer/counter 0 increments every 
** 16 micro-seconds (64/4000000 seconds). A count of 125 gives 
** 2 milliseconds.
** Timer/counter 0 can only give an interrupt on overflow (255 -> 0) 
** so we set the initial counter value (TCNT0) to 256-125 = 131. 
** (Timer/counter 0 does not have any output comparison 
** functionality and it is only 8bits wide.) The first thing the 
** interrupt handler will do is reset the timer value to 131.
**
** It is assumed this function is called before global interrupts
** are enabled. Global interrupts must be enabled for the timer
** system to work.
**
** This function also initialises the data associated with the 
** software timers. (We start with none.)
*/

void init_timer(void) {
	uint8_t timerNum;

	/* Make the timer count every 64 system clock cycles */
	TCCR0 = (0<<CS02)|(1<<CS01)|(1<<CS00);
	/* Set the initial value of the timer/counter */
	TCNT0 = 131;

	/* Enable timer/counter 0 overflow interrupt. Set bit TOIE0
	 * of the TIMSK register.
	 */
	TIMSK |= (1<<TOIE0);
	
	/* Clear the timer/counter 0 overflow flag by writing a 1
	 * to the TOV0 bit of the TIFR register.
	 */
	TIFR = (1<<TOV0); 
	
	/* We start with no software timers */
	for(timerNum = 0; timerNum <= NUM_SW_TIMERS; timerNum++) {
		sw_timer_target[timerNum] = 0;
	}

}

/* Function to register a function to be executed after a given 
** delay - see comment in .h file.
** (If interrupts were on, we disable them while we
** do this and then reenable.)
*/
uint8_t execute_function_once_after_delay(uint16_t delay,
		TimerFunctionType* timerFunction)
{
	uint8_t timerNum;

	if(delay == 0) {
		return 0;
	}

	/* Record whether interrupts are on, then turn interrupts off */
	uint8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();

	/* Iterate over the timers until we find one not in use */
	for(timerNum = NUM_SW_TIMERS; timerNum > 0; timerNum--) {
		if(sw_timer_target[timerNum] == 0) {
			/* Timer is not in use */
			sw_timer_values[timerNum] = 0;	/* Start count for this timer at 0 */
			sw_timer_target[timerNum] = delay;
			sw_timer_functions[timerNum] = timerFunction;
			sw_timer_once_only[timerNum] = 1;
			break;
		}
	}

	/* If interrupts were on when we started, turn them back on */
	if(interrupts_on) {
		sei();
	}
	return timerNum;
}

/* Function to register a function to be called periodically - 
** see comment in .h file. (If interrupts were on, we disable them
** while we do this and then reenable.)
*/
uint8_t execute_function_periodically(uint16_t period, 
		TimerFunctionType* timerFunction)
{
	uint8_t timerNum;

	/* Record whether interrupts are on, then turn interrupts off */
	uint8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();

	/* Use the function above to add a once-only function, then update
	** the field that records whether the timer is once only or periodic.
	*/
	timerNum = execute_function_once_after_delay(period, timerFunction);
	if(timerNum) {
		sw_timer_once_only[timerNum] = 0;
	}

	/* If interrupts were on when we started, turn them back on */
	if(interrupts_on) {
		sei();
	}
	return timerNum;

}


/* Function to stop a software timer. Turn off interrupts
** while we do this and re-enable afterwards.
*/
void cancel_software_timer(uint8_t timerNum)
{
	uint8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();
	sw_timer_target[timerNum] = 0;
	if(interrupts_on) {
		sei();
	}
}

/* Function to get the value of a software timer
*/
uint16_t get_sw_timer_value(uint8_t timerNum)
{
	return sw_timer_values[timerNum];
}

/* 
** Wait for a timer to wrap around back to 0. Interrupts must be on 
** when this is called.
*/
void wait_for(uint16_t delay)
{
	/* Disable interrupts */
	cli();

	sw_timer_values[0] = 0;	/* Start count for this timer at 0 */
	sw_timer_target[0] = delay;
	sw_timer_functions[0] = 0;
	sw_timer_once_only[0] = 1;

	/* Reenable interrupts and wait until our target is reset to 0
	** (This will happen in the ISR) */
	sei();

	while(sw_timer_target[0]) {
		; /* Do nothing */
	}
}

/*
** Interrupt handler for timer 0 overflow. The CPU will clear the 
** overflow flag (TOV0) on calling this handler.
*/
ISR(TIMER0_OVF_vect) {
	uint8_t timerNum;
	/*
	** Reset the timer so the next interrupt happens 
	** at an appropriate time (i.e. in 2ms)
	*/
	TCNT0 = 131;
	
	/*
	** Update our global time variable
	*/
	time+=2;
	
	/* Check our software timers */
	for(timerNum = 0; timerNum <= NUM_SW_TIMERS; timerNum++) {
		/* For each timer, check whether it is running or not */
		if(sw_timer_target[timerNum]) {
			/* Timer is running - increment it */
			sw_timer_values[timerNum] += 2;
			/* If we have passed our target - call the given 
			** function.
			*/
			if(sw_timer_values[timerNum] >= sw_timer_target[timerNum]) {
				/* Call the registered function (if any) */
				if(sw_timer_functions[timerNum]) {
					sw_timer_functions[timerNum]();
				}
				/* Check if this was a once off */
				if(sw_timer_once_only[timerNum]) {
					/* Was once off - cancel the timer */
					sw_timer_target[timerNum] = 0;
				} else {
					/* Wasn't once off - reset the timer */
					sw_timer_values[timerNum] = 0;
				}
			}
		}
	}
}
