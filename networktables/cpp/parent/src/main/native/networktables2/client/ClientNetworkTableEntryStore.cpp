/*
 * ClientNetworkTableEntryStore.cpp
 *
 *  Created on: Nov 2, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/client/ClientNetworkTableEntryStore.h"

/**
 * Create a new ClientNetworkTableEntryStore
 * @param transactionPool
 * @param listenerManager
 */
ClientNetworkTableEntryStore::ClientNetworkTableEntryStore(TableListenerManager& listenerManager): AbstractNetworkTableEntryStore(listenerManager) {}
ClientNetworkTableEntryStore::~ClientNetworkTableEntryStore(){}

bool ClientNetworkTableEntryStore::addEntry(NetworkTableEntry* newEntry){
	{
		NTSynchronized sync(LOCK);
		NetworkTableEntry* entry = (NetworkTableEntry*)namedEntries[newEntry->name];

		if(entry!=NULL){
			if(entry->GetId()!=newEntry->GetId()){
				idEntries.erase(entry->GetId());
				if(newEntry->GetId()!=NetworkTableEntry::UNKNOWN_ID){
					entry->SetId(newEntry->GetId());
					idEntries[newEntry->GetId()] = entry;
				}
			}
			
			entry->ForcePut(newEntry->GetSequenceNumber(), newEntry->GetType(), newEntry->GetValue());
		}
		else{
			if(newEntry->GetId()!=NetworkTableEntry::UNKNOWN_ID)
				idEntries[newEntry->GetId()] = newEntry;
			namedEntries[newEntry->name] = newEntry;
		}
	}
	return true;
}

bool ClientNetworkTableEntryStore::updateEntry(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value) {
	{ 
		NTSynchronized sync(LOCK);
		entry->ForcePut(sequenceNumber, value);
		if(entry->GetId()==NetworkTableEntry::UNKNOWN_ID){
			return false;
		}
		return true;
	}
}

/**
 * Send all unknown entries in the entry store to the given connection
 * @param connection
 * @throws IOException
 */
void ClientNetworkTableEntryStore::sendUnknownEntries(NetworkTableConnection& connection) {
	{ 
		NTSynchronized sync(LOCK);
		std::map<std::string, NetworkTableEntry*>::iterator itr;
		for(itr = namedEntries.begin(); itr != namedEntries.end(); itr++)
		{
			NetworkTableEntry* entry = (*itr).second;
			if(entry->GetId()==NetworkTableEntry::UNKNOWN_ID)
				connection.sendEntryAssignment(*entry);
		}
		connection.flush();
	}
}
