/*
 * IllegalStateException.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/util/IllegalStateException.h"

IllegalStateException::IllegalStateException(const char* msg) 
	: message(msg)
{
}
IllegalStateException::~IllegalStateException() throw ()
{	
}
