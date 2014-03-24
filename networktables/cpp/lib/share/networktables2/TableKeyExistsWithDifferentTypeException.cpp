/*
 * TableKeyExistsWithDifferentKeyException.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/TableKeyExistsWithDifferentTypeException.h"



TableKeyExistsWithDifferentTypeException::TableKeyExistsWithDifferentTypeException(const std::string, NetworkTableEntryType* existingType) {
	//TODO construct string this(existingKey, existingType, "");
}

TableKeyExistsWithDifferentTypeException::TableKeyExistsWithDifferentTypeException(const std::string, NetworkTableEntryType* existingType, const char* message) {
	//TODO construct string super("Illegal put - key '" + existingKey + "' exists with type '" + existingType + "'. "+message);
}

TableKeyExistsWithDifferentTypeException::~TableKeyExistsWithDifferentTypeException() throw (){
	//TODO delete message
}
