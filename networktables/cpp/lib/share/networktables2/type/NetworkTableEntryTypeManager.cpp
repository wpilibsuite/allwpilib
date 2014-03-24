/*
 * NetworkTableEntryTypeManager.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/NetworkTableEntryTypeManager.h"
#include "networktables2/type/DefaultEntryTypes.h"


NetworkTableEntryType* NetworkTableEntryTypeManager::GetType(TypeId id){
	return entryTypes[id];
}

void NetworkTableEntryTypeManager::RegisterType(NetworkTableEntryType& type){
	entryTypes[type.id] = &type;
}

NetworkTableEntryTypeManager::NetworkTableEntryTypeManager(){
	for(int i = 0; i<MAX_NUM_TABLE_ENTRY_TYPES; ++i)
		entryTypes[i] = NULL;
	
	DefaultEntryTypes::registerTypes(this);
}
