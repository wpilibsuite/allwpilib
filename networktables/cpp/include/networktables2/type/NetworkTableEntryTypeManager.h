/*
 * NetworkTableEntryTypeManager.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLEENTRYTYPEMANAGER_H_
#define NETWORKTABLEENTRYTYPEMANAGER_H_

class NetworkTableEntryTypeManager;

#include "networktables2/type/NetworkTableEntryType.h"

class NetworkTableEntryTypeManager
{
private:
	NetworkTableEntryType* entryTypes[MAX_NUM_TABLE_ENTRY_TYPES];
public:
	/**
	 * Creates a new NetworkTableEntryTypeManager, with the default
	 * types registered under their respective TypeID's, and all other
	 * TypeID's null.
	 */
	NetworkTableEntryTypeManager();
	
	/**
	 * Retrieves the entry type associated with the given
	 * type id, if one exists.
	 * 
	 * @param type The identifier by which to retrieve a type.
	 * @return The NetworkTableEntryType associated with the given
	 * 	type identifier, if one exists. Otherwise, null.
	 */
	NetworkTableEntryType* GetType(TypeId type);
	
	/**
	 * Registers the given NetworkTableEntryType with this type
	 * manager, such that is is returned by subsequent calls
	 * to GetType with the appropriate ID.
	 * 
	 * @param type The type to register.
	 */
	void RegisterType(NetworkTableEntryType& type);
};

#endif /* NETWORKTABLEENTRYTYPEMANAGER_H_ */
