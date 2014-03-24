/*
 * AbstractNetworkTableEntryStore.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/AbstractNetworkTableEntryStore.h"
#include "networktables2/TableKeyExistsWithDifferentTypeException.h"
#include <map>
#include <vector>
#include <iostream>
#include <stdio.h>

	AbstractNetworkTableEntryStore::AbstractNetworkTableEntryStore(TableListenerManager& lstnManager): 
			listenerManager(lstnManager){
	}
	
	AbstractNetworkTableEntryStore::~AbstractNetworkTableEntryStore(){
		LOCK.take();
		std::map<std::string, NetworkTableEntry*>::iterator itr;
		for(itr = namedEntries.begin(); itr != namedEntries.end();){
		    delete itr->second;
		    namedEntries.erase(itr++);
		}
	}
	
	/**
	 * Get an entry based on it's name
	 * @param name the name of the entry to look for
	 * @return the entry or null if the entry does not exist
	 */
	NetworkTableEntry* AbstractNetworkTableEntryStore::GetEntry(std::string& name){
		{ 
			NTSynchronized sync(LOCK);
			std::map<std::string, NetworkTableEntry*>::iterator value_itr = namedEntries.find(name);
			if(value_itr != namedEntries.end()) {
				return value_itr->second;
			}
			return NULL;
		}
	}
	
	NetworkTableEntry* AbstractNetworkTableEntryStore::GetEntry(EntryId entryId){
		{ 
			NTSynchronized sync(LOCK);
			
			std::map<EntryId, NetworkTableEntry*>::iterator value_itr = idEntries.find(entryId);
			if(value_itr != idEntries.end()) {
				return value_itr->second;
			}
			return NULL;
		}
	}
	
	std::vector<std::string>* AbstractNetworkTableEntryStore::keys(){
		{ 
			NTSynchronized sync(LOCK);
			std::vector<std::string>* keys = new std::vector<std::string>();
			std::map<std::string, NetworkTableEntry*>::iterator itr;
			
			for(itr = namedEntries.begin(); itr != namedEntries.end(); itr++)
			{
				std::string key = (*itr).first;
				keys->push_back(key);
			}
			
			return (keys);
		}
		
	}
	
	/**
	 * Remove all entries
         * NOTE: This method should not be used with applications which cache entries which would lead to unknown results
         * This method is for use in testing only
	 */
	void AbstractNetworkTableEntryStore::clearEntries(){
		{ 
			NTSynchronized sync(LOCK);
			namedEntries.clear();
			idEntries.clear();
		}
	}
	
	/**
	 * clear the id's of all entries
	 */
	void AbstractNetworkTableEntryStore::clearIds(){
		{ 
			NTSynchronized sync(LOCK);
			std::map<std::string, NetworkTableEntry*>::iterator itr;
			idEntries.clear();
			
			for(itr = namedEntries.begin(); itr != namedEntries.end(); itr++)
			{
				((NetworkTableEntry*)(*itr).second)->ClearId();
			}
		}
	}
	
	void AbstractNetworkTableEntryStore::SetOutgoingReceiver(OutgoingEntryReceiver* receiver){
		outgoingReceiver = receiver;
	}
	
	void AbstractNetworkTableEntryStore::SetIncomingReceiver(OutgoingEntryReceiver* receiver){
		incomingReceiver = receiver;
	}
	
	/**
	 * Stores the given value under the given name and queues it for 
	 * transmission to the server.
	 * 
	 * @param name The name under which to store the given value.
	 * @param type The type of the given value.
	 * @param value The value to store.
	 * @throws TableKeyExistsWithDifferentTypeException Thrown if an 
	 *  entry already exists with the given name and is of a different type.
	 */
	void AbstractNetworkTableEntryStore::PutOutgoing(std::string& name, NetworkTableEntryType* type, EntryValue value){
		{ 
			NTSynchronized sync(LOCK);
			std::map<std::string, NetworkTableEntry*>::iterator index = namedEntries.find(name);
			NetworkTableEntry* tableEntry;
			if(index == namedEntries.end())//if the name does not exist in the current entries
			  {
				tableEntry = new NetworkTableEntry(name, type, value);
				if(addEntry(tableEntry))
				{
					tableEntry->FireListener(listenerManager);
					outgoingReceiver->offerOutgoingAssignment(tableEntry);
				}
			}
			else
			{
				tableEntry = index->second;
				if(tableEntry->GetType()->id != type->id){
					throw TableKeyExistsWithDifferentTypeException(name, tableEntry->GetType());
				}
				
				EntryValue oldValue = tableEntry->GetValue();
				if(!type->areEqual(value, oldValue)){
				  if(updateEntry(tableEntry, (SequenceNumber)(tableEntry->GetSequenceNumber() + 1), value)){
				    outgoingReceiver->offerOutgoingUpdate(tableEntry);
				  }
				
				  tableEntry->FireListener(listenerManager);
				}
			}
		}
	}
	
	void AbstractNetworkTableEntryStore::PutOutgoing(NetworkTableEntry* tableEntry, EntryValue value){
		{ 
			NTSynchronized sync(LOCK);
			NetworkTableEntryType* type = tableEntry->GetType();
			EntryValue oldValue = tableEntry->GetValue();
			if(!type->areEqual(value, oldValue)){
			  if(updateEntry(tableEntry, (SequenceNumber)(tableEntry->GetSequenceNumber() + 1), value)){
			    outgoingReceiver->offerOutgoingUpdate(tableEntry);
			  }
			
			  tableEntry->FireListener(listenerManager);
			}
		}
	}
	
	void AbstractNetworkTableEntryStore::offerIncomingAssignment(NetworkTableEntry* entry){
		{ 
			NTSynchronized sync(LOCK);
			std::map<std::string, NetworkTableEntry*>::iterator itr = namedEntries.find(entry->name);
			NetworkTableEntry* tableEntry;
			if(addEntry(entry)){
				if(itr != namedEntries.end()){
					tableEntry = itr->second;
				}
				else{
					tableEntry = entry;
				}
				
				tableEntry->FireListener(listenerManager);//if we didnt have a pointer, then the copy of the version in the list would call this method, however with the pointer we are updating the version in the list
				incomingReceiver->offerOutgoingAssignment(tableEntry);
			}
			else
			  delete entry;
		}
	}
	
	void AbstractNetworkTableEntryStore::offerIncomingUpdate(NetworkTableEntry* entry, SequenceNumber squenceNumber, EntryValue value){
		{ 
			NTSynchronized sync(LOCK);
			if(updateEntry(entry, squenceNumber, value)){
				entry->FireListener(listenerManager);
				incomingReceiver->offerOutgoingUpdate(entry);
			}
		}
	}
	
	/**
	 * Called to say that a listener should notify the listener manager of all of the entries
	 * @param listener
	 * @param table 
	 */
	void AbstractNetworkTableEntryStore::notifyEntries(ITable* table, ITableListener* listener){
		{ 
			NTSynchronized sync(LOCK);
			std::map<std::string, NetworkTableEntry*>::iterator itr;
			for(itr = namedEntries.begin(); itr != namedEntries.end(); itr++)
			{
				NetworkTableEntry* entry = itr->second;
				listener->ValueChanged(table, itr->first, entry->GetValue(), true);
			}
		}
	}
