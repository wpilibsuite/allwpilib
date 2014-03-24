/*
 * ClientNetworkTableEntryStore.h
 *
 *  Created on: Nov 2, 2012
 *      Author: Mitchell Wills
 */

#ifndef CLIENTNETWORKTABLEENTRYSTORE_H_
#define CLIENTNETWORKTABLEENTRYSTORE_H_


class ClientNetworkTableEntryStore;


#include "networktables2/AbstractNetworkTableEntryStore.h"
#include "networktables2/NetworkTableEntry.h"
#include "OSAL/Synchronized.h"

/**
 * The entry store for a {@link NetworkTableClient}
 * 
 * @author Mitchell
 *
 */
class ClientNetworkTableEntryStore : public AbstractNetworkTableEntryStore{

public:
	/**
	 * Create a new ClientNetworkTableEntryStore
	 * @param transactionPool
	 * @param listenerManager
	 */
	ClientNetworkTableEntryStore(TableListenerManager& listenerManager);
	virtual ~ClientNetworkTableEntryStore();
	
protected:
	bool addEntry(NetworkTableEntry* newEntry);
	bool updateEntry(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value);

public:
	/**
	 * Send all unknown entries in the entry store to the given connection
	 * @param connection
	 * @throws IOException
	 */
	void sendUnknownEntries(NetworkTableConnection& connection);
};



#endif /* CLIENTNETWORKTABLEENTRYSTORE_H_ */
