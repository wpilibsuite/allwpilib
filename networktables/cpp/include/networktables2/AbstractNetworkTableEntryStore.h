/*
 * AbstractNetworkTableEntryStore.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef ABSTRACTNETWORKTABLEENTRYSTORE_H_
#define ABSTRACTNETWORKTABLEENTRYSTORE_H_



class TableListenerManager;
class AbstractNetworkTableEntryStore;


#include "OSAL/Synchronized.h"
#include <string>
#include "networktables2/NetworkTableEntry.h"
#include "networktables2/IncomingEntryReceiver.h"
#include "networktables2/OutgoingEntryReceiver.h"
#include "networktables2/type/NetworkTableEntryType.h"
#include "tables/ITable.h"
#include "tables/ITableListener.h"
#include <map>
#include <vector>



class TableListenerManager{
public:
	virtual ~TableListenerManager(){}
	virtual void FireTableListeners(std::string& name, EntryValue value, bool isNew) = 0;
};

class AbstractNetworkTableEntryStore : public IncomingEntryReceiver{
protected:
	std::map<EntryId,NetworkTableEntry*> idEntries;
	std::map<std::string,NetworkTableEntry*> namedEntries;
	TableListenerManager& listenerManager;

	AbstractNetworkTableEntryStore(TableListenerManager& lstnManager);

	OutgoingEntryReceiver* outgoingReceiver;
	OutgoingEntryReceiver* incomingReceiver;


	
	virtual bool addEntry(NetworkTableEntry* entry) = 0;
	virtual bool updateEntry(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value) = 0;
	
public:
	virtual ~AbstractNetworkTableEntryStore();

	NTReentrantSemaphore LOCK;
	NetworkTableEntry* GetEntry(EntryId entryId);
	NetworkTableEntry* GetEntry(std::string& name);

	std::vector<std::string>* keys();
    void clearEntries();
	void clearIds();

	void SetOutgoingReceiver(OutgoingEntryReceiver* receiver);
	void SetIncomingReceiver(OutgoingEntryReceiver* receiver);
	
	void PutOutgoing(std::string& name, NetworkTableEntryType* type, EntryValue value);
	void PutOutgoing(NetworkTableEntry* tableEntry, EntryValue value);
	

	void offerIncomingAssignment(NetworkTableEntry* entry);
	void offerIncomingUpdate(NetworkTableEntry* entry, EntryId sequenceNumber, EntryValue value);
	
	void notifyEntries(ITable* table, ITableListener* listener);

};


#endif /* ABSTRACTNETWORKTABLEENTRYSTORE_H_ */
