/*
** timer.h
**
** Written by Peter Sutton
*/

/* Guard band to ensure this definition is only included once */
#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

/*
** Initialise AVR timer 0 - which provides our regular timebase. Global
** interrupts must be enabled for this to work. After this is called
** timer/counter 0 will overflow every 2 milliseconds. It is possible
** to register events to happen on various values of this clock
** (see functions below).
*/
void init_timer(void);

/*
** The global time variable counts milliseconds (in multiples of 2) 
** from 0 up to the overflow point (when it is reset to 0). 
** This variable can be read if desired, but should not be written 
** outside the timer module.
*/
extern volatile uint16_t time;

/*
** There are a fixed number of software timers based on this clock. 
** The timer numbers range from 1 to NUM_SW_TIMERS. (This number
** can be adjusted if necessary but must fit in a uint8_t type.)
*/
#define NUM_SW_TIMERS 8

/* The following type definition is the type of functions that can
** be registered to be executed periodically, i.e., such functions
** must take no arguments and return nothing.
*/
typedef void TimerFunctionType(void);

/* The functions below allow software timers to be set, i.e., the
** programmer can specify that certain functions are executed 
** periodically or only once after a certain delay. Delays and
** periods specified should be a multiple of 2ms. (If not, they will
** effectively be rounded up to a multiple of 2.). Note that the 
** number of software timers is limited (as defined above). 
**
** Note that the delay/period is based on clock ticks, not elapsed
** time, and a clock tick may happen at any time in the next 2ms
** after you call the function. It is possible that any delay requested
** could end up being up to 2ms shorter. Similarly, for a request
** to execute a periodic function - the very first period may be 
** up to 2ms shorter, but future periods will be as requested.
**
** The execute_function... functions return the software timer
** number (1 to NUM_SW_TIMERS) that the request has been assigned to
** or 0 if there are no free software timers and the request is not
** possible. If a delay/period of 0 is requested, then the request
** is ignored and 0 is returned.
**
** Once requested, a timer action (function execution) can be cancelled
** by calling cancel_timer_request() with the software timer number
** as the argument.
**
** These functions assume that the hardware timer is operating (i.e.
** AVR timer/counter 0) along with the associated interrupt handler.
**
** Note that the registered functions will be executed from within
** an interrupt service routine and should NOT access/modify
** variables that can also be modified from non-ISR code unless
** interrupts are disabled whilst the variables are modified from 
** non-ISR code! Because the functions are executed during ISRs
** they should be kept as short as possible - perhaps just setting
** a flag that can be checked for from non-ISR code.
*/

/* execute_function_once_after_delay(delay, functionName)
**
** Specify a function to be executed after the given delay (ms). The
** function returns the timer number that is being used (between 1
** and NUM_SW_TIMERS inclusive) or 0 if the request can't be met.
** delay should be between 2 and 65534 inclusive.
*/
uint8_t execute_function_once_after_delay(uint16_t delay, 
		TimerFunctionType* timerFunction);

/* execute_function_periodically(period, functionName)
**
** Specify a function to be executed every so often, with the given
** period (ms). The function returns the timer number that is being used
** (between 1 and NUM_SW_TIMERS inclusive) or 0 if the request can't be met.
** period should be between 2 and 65534 inclusive.
*/
uint8_t execute_function_periodically(uint16_t period, 
		TimerFunctionType* timerFunction);

/* wait_for(time)
**
** Wait (do nothing) for a given time period. Interrupts will be enabled 
** by this function if they weren't already enabled. The delay (ms) will 
** be rounded up to a multiple of 2 milliseconds. This function
** may return up to 2ms earlier than requested, depending on when this
** function is called with respect the current clock.
*/
void wait_for(uint16_t delay);

/* cancel_software_timer(timerNumber)
**
** Stop the given software timer. The registered function will not be
** called, and the software timer is free to be reused.
*/
void cancel_software_timer(uint8_t timerNum);

/* get_sw_timer_value(timerNumber)
**
** Get the given software timer value (in ms). The timerNum should be 
** a timer number (between 1 and NUM_SW_TIMERS inclusive) that has been 
** returned by execute_function_once_after_delay() or 
** execute_function_periodically().
*/
uint16_t get_sw_timer_value(uint8_t timerNum);

#endif
