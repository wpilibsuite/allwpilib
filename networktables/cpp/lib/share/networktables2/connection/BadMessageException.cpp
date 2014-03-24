/*
 * BadMessageException.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/connection/BadMessageException.h"

BadMessageException::BadMessageException(const char* msg)
	: message(msg)
{

}

BadMessageException::~BadMessageException() throw ()
{
}

const char* BadMessageException::what()
{
	return message.c_str();
}
