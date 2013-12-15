/*
 * BadMessageException.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef BADMESSAGEEXCEPTION_H_
#define BADMESSAGEEXCEPTION_H_

#include <exception>
#include <string>

class BadMessageException : public std::exception
{
public:
	BadMessageException(const char* message);
	~BadMessageException() throw ();
	const char* what();
private:
	std::string message;
};

#endif /* BADMESSAGEEXCEPTION_H_ */
