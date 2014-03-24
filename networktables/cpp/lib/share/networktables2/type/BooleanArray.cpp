/*
 * BooleanArray.cpp
 *
 *  Created on: Nov 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/BooleanArray.h"
#include "networktables2/type/DefaultEntryTypes.h"


const TypeId BooleanArray::BOOLEAN_ARRAY_RAW_ID = 0x10;
ArrayEntryType BooleanArray::TYPE(BOOLEAN_ARRAY_RAW_ID, DefaultEntryTypes::BOOLEAN);

BooleanArray::BooleanArray() : ArrayData(TYPE) {
}

bool BooleanArray::get(int index){
	return _get(index).b;
}

void BooleanArray::set(int index, bool value){
	EntryValue eValue;
	eValue.b = value;
	_set(index, eValue);
}

void BooleanArray::add(bool value){
	EntryValue eValue;
	eValue.b = value;
	_add(eValue);
}
