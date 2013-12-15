/*
 * System.h
 * 
 * For some platform specific code related to the system
 *
 *  Created on: Sep 25, 2012
 *      Author: Mitchell Wills
 */

#ifndef TIME_H_
#define TIME_H_

/**
 * Causes the current thread to sleep at least the
 * given number of milliseconds.
 * 
 * @param ms The number of milliseconds to sleep.
 */
void sleep_ms(unsigned long ms);

/**
 * Retrieves the current time in milliseconds.
 * 
 * @return The current time in milliseconds.
 */
unsigned long currentTimeMillis();

/**
 * Writes a warning message to the standard error
 * stream.
 * 
 * @param message The string message to write.
 */
void writeWarning(const char* message);


#endif /* TIME_H_ */
