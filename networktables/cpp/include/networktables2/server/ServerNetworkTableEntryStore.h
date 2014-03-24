/*
 * ServerNetworkTableEntryStore.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#ifndef SERVERNETWORKTABLEENTRYSTORE_H_
#define SERVERNETWORKTABLEENTRYSTORE_H_


class ServerNetworkTableEntryStore;


#include "networktables2/AbstractNetworkTableEntryStore.h"
#include "networktables2/NetworkTableEntry.h"
#include "OSAL/Synchronized.h"



/**
 * The entry store for a {@link NetworkTableServer}
 * 
 * @author Mitchell
 *
 */
class ServerNetworkTableEntryStore : public AbstractNetworkTableEntryStore{
private:
	EntryId nextId;
public:
	/**
	 * Create a new Server entry store
	 * @param transactionPool the transaction pool
	 * @param listenerManager the listener manager that fires events from this entry store
	 */
	ServerNetworkTableEntryStore(TableListenerManager& listenerManager);
	virtual ~ServerNetworkTableEntryStore();
	
protected:
	bool addEntry(NetworkTableEntry* newEntry);

	bool updateEntry(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value);
	
public:
	/**
	 * Send all entries in the entry store as entry assignments in a single transaction
	 * @param connection
	 * @throws IOException
	 */
	void sendServerHello(NetworkTableConnection& connection);
};



#endif /* SERVERNETWORKTABLEENTRYSTORE_H_ */
