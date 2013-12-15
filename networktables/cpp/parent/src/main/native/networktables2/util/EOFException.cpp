/*
 * EOFException.cpp
 *
 *  Created on: Oct 1, 2012
 *      Author: Mitchell Wills
 */
#include "networktables2/util/EOFException.h"

EOFException::EOFException() : IOException("End of File"){}
EOFException::~EOFException() throw(){}

bool EOFException::isEOF(){
	return true;
}
