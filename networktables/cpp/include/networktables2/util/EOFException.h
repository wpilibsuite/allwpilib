/*
 * EOFException.h
 *
 *  Created on: Oct 1, 2012
 *      Author: Mitchell Wills
 */

#ifndef EOFEXCEPTION_H_
#define EOFEXCEPTION_H_

#include "networktables2/util/IOException.h"


/**
 * Indicates that an EOF was encountered during an I/O operation,
 * and therefore the operation could not be completed. 
 */
class EOFException : public IOException{
public:
	/**
	 * Creates an EOFException.
	 */
	EOFException();
	virtual ~EOFException() throw ();
	
	/**
	 * Implements {@link IOException}::isEOF() 
	 */
	virtual bool isEOF();
};




#endif /* EOFEXCEPTION_H_ */
