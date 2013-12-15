/*
 * ConnectionAdapter.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef CONNECTIONADAPTER_H_
#define CONNECTIONADAPTER_H_

class ConnectionAdapter;

#include "networktables2/NetworkTableEntry.h"
#include "networktables2/connection/BadMessageException.h"
#include "networktables2/util/IOException.h"
#include "tables/ITable.h"

class ConnectionAdapter
{
public:
	virtual ~ConnectionAdapter()
	{
	}
	//returns true if the connection should still be alive
	virtual bool keepAlive() = 0;
	virtual void clientHello(ProtocolVersion protocolRevision) = 0;
	virtual void serverHelloComplete() = 0;
	virtual void protocolVersionUnsupported(ProtocolVersion protocolRevision) = 0;
	virtual void offerIncomingAssignment(NetworkTableEntry* newEntry) = 0;
	virtual void offerIncomingUpdate(NetworkTableEntry* newEntry, SequenceNumber sequenceNumber, EntryValue value) = 0;
	virtual NetworkTableEntry* GetEntry(EntryId) = 0;

	/**
	 * called if a bad message exception is thrown
	 * @param e
	 */
	virtual void badMessage(BadMessageException& e) = 0;

	/**
	 * called if an io exception is thrown
	 * @param e
	 */
	virtual void ioException(IOException& e) = 0;
};

#endif /* CONNECTIONADAPTER_H_ */
