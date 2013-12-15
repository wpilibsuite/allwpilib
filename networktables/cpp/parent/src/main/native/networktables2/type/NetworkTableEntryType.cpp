/*
 * NetworkTableEntryType.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/NetworkTableEntryType.h"


NetworkTableEntryType::NetworkTableEntryType(TypeId _id, const char* _name) : id(_id), name(_name){
}
NetworkTableEntryType::~NetworkTableEntryType(){
}

  EntryValue NetworkTableEntryType::copyValue(EntryValue value){
    return value;
  }  
  void NetworkTableEntryType::deleteValue(EntryValue value){
  }


bool NetworkTableEntryType::isComplex(){
	return false;
}
