/*
 * IOException.cpp
 *
 *  Created on: Oct 1, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/util/IOException.h"
#include <stdlib.h>
#include <string.h>


IOException::IOException(const char* msg) : message(strdup(msg)), errorValue(0){}
IOException::IOException(const char* msg, int _errorValue) : message(strdup(msg)), errorValue(_errorValue){}

const char* IOException::what(){
	return message;
}

bool IOException::isEOF(){return false;}

IOException::~IOException() throw (){
	free((void*)message);
}
