/*
 * NumberArray.cpp
 *
 *  Created on: Nov 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/NumberArray.h"
#include "networktables2/type/DefaultEntryTypes.h"


const TypeId NumberArray::NUMBER_ARRAY_RAW_ID = 0x11;
ArrayEntryType NumberArray::TYPE(NUMBER_ARRAY_RAW_ID, DefaultEntryTypes::DOUBLE);

NumberArray::NumberArray() : ArrayData(TYPE) {
}

double NumberArray::get(int index){
	return _get(index).f;
}

void NumberArray::set(int index, double value){
	EntryValue eValue;
	eValue.f = value;
	_set(index, eValue);
}

void NumberArray::add(double value){
	EntryValue eValue;
	eValue.f = value;
	_add(eValue);
}
