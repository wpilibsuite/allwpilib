/*
 * StringArray
 *
 *  Created on: Nov 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/StringArray.h"
#include "networktables2/type/DefaultEntryTypes.h"


const TypeId StringArray::STRING_ARRAY_RAW_ID = 0x12;
ArrayEntryType StringArray::TYPE(STRING_ARRAY_RAW_ID, DefaultEntryTypes::STRING);

StringArray::StringArray() : ArrayData(TYPE) {
}

std::string StringArray::get(int index){
	return *(std::string*)_get(index).ptr;
}

void StringArray::set(int index, std::string value){
	EntryValue eValue;
	eValue.ptr = &value;
	_set(index, eValue);
}

void StringArray::add(std::string value){
	EntryValue eValue;
	eValue.ptr = &value;
	_add(eValue);
}
