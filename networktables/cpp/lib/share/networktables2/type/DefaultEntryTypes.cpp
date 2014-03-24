/*
 * DefaultEntryTypes.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/DefaultEntryTypes.h"
#include "networktables2/type/NetworkTableEntryType.h"
#include "networktables2/connection/DataIOStream.h"
#include "networktables2/type/BooleanArray.h"
#include "networktables2/type/NumberArray.h"
#include "networktables2/type/StringArray.h"


DefaultEntryTypes::BOOLEAN_t DefaultEntryTypes::BOOLEAN;
DefaultEntryTypes::DOUBLE_t DefaultEntryTypes::DOUBLE;
DefaultEntryTypes::STRING_t DefaultEntryTypes::STRING;

DefaultEntryTypes::BOOLEAN_t::BOOLEAN_t() : NetworkTableEntryType(BOOLEAN_RAW_ID, "Boolean"){}
void DefaultEntryTypes::BOOLEAN_t::sendValue(EntryValue value, DataIOStream& os) {
	os.writeByte(value.b);
}
EntryValue DefaultEntryTypes::BOOLEAN_t::readValue(DataIOStream& is) {
	EntryValue value;
	value.b = (is.readByte()!=0);
	return value;
}
bool DefaultEntryTypes::BOOLEAN_t::areEqual(EntryValue v1, EntryValue v2) {
  return v1.b == v2.b;
}
	
DefaultEntryTypes::DOUBLE_t::DOUBLE_t() : NetworkTableEntryType(DOUBLE_RAW_ID, "Double"){}
void DefaultEntryTypes::DOUBLE_t::sendValue(EntryValue eValue, DataIOStream& os) {
	uint64_t value = *reinterpret_cast<uint64_t*>(&eValue.f);
	for(int i = 0; i<8; ++i){
		os.writeByte((value>>56)&0xFF);
		value<<=8;
	}
}
EntryValue DefaultEntryTypes::DOUBLE_t::readValue(DataIOStream& is) {
	uint64_t value = 0;
	
	for(int i = 0; i<8; ++i){
		value<<=8;
		value |= (is.readByte()&0xFF);
	}
	
	EntryValue eValue;
	eValue.f = *reinterpret_cast<double*>(&value);
	return eValue;
}
bool DefaultEntryTypes::DOUBLE_t::areEqual(EntryValue v1, EntryValue v2) {
  return v1.f == v2.f;
}
	
DefaultEntryTypes::STRING_t::STRING_t() : NetworkTableEntryType(STRING_RAW_ID, "String"){}
void DefaultEntryTypes::STRING_t::sendValue(EntryValue value, DataIOStream& os) {
	os.writeString(*(std::string*)value.ptr);
}
EntryValue DefaultEntryTypes::STRING_t::readValue(DataIOStream& is) {
	EntryValue value;
	value.ptr = is.readString();
	return value;
}
EntryValue DefaultEntryTypes::STRING_t::copyValue(EntryValue value){
  if(value.ptr==NULL)
    return value;
	EntryValue newValue;
	newValue.ptr = new std::string(*((std::string*)value.ptr));
	return newValue;
}
void DefaultEntryTypes::STRING_t::deleteValue(EntryValue value){
  if(value.ptr!=NULL)
    delete (std::string*)value.ptr;
}
bool DefaultEntryTypes::STRING_t::areEqual(EntryValue v1, EntryValue v2) {
  std::string* str1 = (std::string*)v1.ptr;
  std::string* str2 = (std::string*)v2.ptr;
  return str1->compare(*str2)==0;
}



void DefaultEntryTypes::registerTypes(NetworkTableEntryTypeManager* manager){
	manager->RegisterType(BOOLEAN);
	manager->RegisterType(DOUBLE);
	manager->RegisterType(STRING);
	manager->RegisterType(BooleanArray::TYPE);
	manager->RegisterType(NumberArray::TYPE);
	manager->RegisterType(StringArray::TYPE);
}




