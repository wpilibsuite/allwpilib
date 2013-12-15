/*
 * ComplexEntryType.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/ComplexEntryType.h"


ComplexEntryType::ComplexEntryType(TypeId id, const char* name) : NetworkTableEntryType(id, name){}

bool ComplexEntryType::isComplex(){
	return true;
}
