/*
 * ServerNetworkTableEntryStore.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/server/ServerNetworkTableEntryStore.h"

ServerNetworkTableEntryStore::ServerNetworkTableEntryStore(TableListenerManager& _listenerManager) :
	AbstractNetworkTableEntryStore(_listenerManager)
{
	nextId = (EntryId)0;
}
ServerNetworkTableEntryStore::~ServerNetworkTableEntryStore()
{
}

bool ServerNetworkTableEntryStore::addEntry(NetworkTableEntry* newEntry)
{
	NTSynchronized sync(LOCK);
	NetworkTableEntry* entry = namedEntries[newEntry->name];
	
	if (entry == NULL)
	{
		newEntry->SetId(nextId++);
		idEntries[newEntry->GetId()] = newEntry;
		namedEntries[newEntry->name] = newEntry;
		return true;
	}
	return false;
}

bool ServerNetworkTableEntryStore::updateEntry(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value)
{
	NTSynchronized sync(LOCK);
	return entry->PutValue(sequenceNumber, value);
}

/**
 * Send all entries in the entry store as entry assignments in a single transaction
 * @param connection
 * @throws IOException
 */
void ServerNetworkTableEntryStore::sendServerHello(NetworkTableConnection& connection)
{
	NTSynchronized sync(LOCK);
	std::map<std::string, NetworkTableEntry*>::iterator itr;
	for (itr = namedEntries.begin(); itr != namedEntries.end(); itr++)
	{
		NetworkTableEntry* entry = itr->second;
		connection.sendEntryAssignment(*entry);
	}
	connection.sendServerHelloComplete();
	connection.flush();
}
