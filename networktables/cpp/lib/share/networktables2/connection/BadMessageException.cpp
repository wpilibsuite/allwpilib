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

BadMessageException::~BadMessageException() noexcept
{
}

const char* BadMessageException::what() const noexcept
{
	return message.c_str();
}
