/*
 * FILE: serialio.h
 *
 * Written by Peter Sutton.
 * 
 * Module to allow standard input/output routines to be used via 
 * serial port. The init_serial_stdio() method must be called before
 * any standard IO methods (e.g. printf). We use interrupt-based serial
 * IO and a circular buffer to store output messages. (This allows us 
 * to print many characters at once to the buffer and have them 
 * output by the UART as speed permits.) Interrupts must be enabled 
 * globally for this module to work.
 *
 */

/* Guard band to ensure this definition is only included once */
#ifndef SERIALIO_H
#define SERIALIO_H

/*
** Initialise serial IO using the UART. baudrate specifies the desired
** baudrate (e.g. 19200) and echo determines whether incoming characters
** echoed back to the UART output as they are received (zero means no
** echo, non-zero means echo)
*/
void init_serial_stdio(long baudrate, char echo);

/*
** Test if input is available from the serial port. Return 0 if not,
** non-zero otherwise.
*/
int8_t input_available(void);

#endif
