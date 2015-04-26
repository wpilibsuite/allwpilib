/*
 * IllegalStateException.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef ILLEGALSTATEEXCEPTION_H_
#define ILLEGALSTATEEXCEPTION_H_

#include "NTBase.h"
#include <exception>
#include <string>

class IllegalStateException : public std::exception{
public:
	IllegalStateException(const char* message);
	const char* what() const noexcept {return message.c_str();};
	~IllegalStateException() throw ();
private:
	std::string message;
};

#endif /* ILLEGALSTATEEXCEPTION_H_ */
