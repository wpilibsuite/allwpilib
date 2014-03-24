/*
 * IOException.h
 *
 *  Created on: Oct 1, 2012
 *      Author: Mitchell Wills
 */

#ifndef IOEXCEPTION_H_
#define IOEXCEPTION_H_

#include <exception>

/**
 * Inidcates that an unrecoverable I/O failure occured.
 */
class IOException : public std::exception{
public:
	/**
	 * Creates a new IOException with the given message.
	 * 
	 * @param message The message to associate with this exception.
	 */
	IOException(const char* message);
	
	/**
	 * Creates a new IOException with the given message and
	 * error value.
	 * 
	 * @param message The message to associate with this exception.
	 * @param errorValue The integer code to associate with this exception.
	 */
	IOException(const char* message, int errorValue);
	
	/**
	 * Gets the message associated with this exception.
	 * 
	 * @return The message associated with this exception.
	 */
	const char* what();
	
	/**
	 * Determines whether this exception indicates that an EOF
	 * was encountered.
	 * 
	 * @return True if this exception indicates that an EOF was encountered.
	 * 	False otherwise.
	 */
	virtual bool isEOF();
	virtual ~IOException() throw ();
private:
	const char* message;
	int errorValue;
};


#endif /* IOEXCEPTION_H_ */
