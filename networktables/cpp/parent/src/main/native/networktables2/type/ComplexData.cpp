/*
 * ComplexData.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/ComplexData.h"


ComplexData::ComplexData(ComplexEntryType& _type) : type(_type){}
	
ComplexEntryType& ComplexData::GetType() {
	return type;
}
