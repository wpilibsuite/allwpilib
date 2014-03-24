/*
 * IncomingEntryReceiver.h
 *
 *  Created on: Sep 19, 2012
 *      Author: Mitchell Wills
 */

#ifndef INCOMINGENTRYRECEIVER_H_
#define INCOMINGENTRYRECEIVER_H_

#include "networktables2/NetworkTableEntry.h"
#include "tables/ITable.h"

class IncomingEntryReceiver
{
public:
	virtual ~IncomingEntryReceiver()
	{
	}
	virtual void offerIncomingAssignment(NetworkTableEntry* entry) = 0;
	virtual void offerIncomingUpdate(NetworkTableEntry* entry, SequenceNumber entrySequenceNumber, EntryValue value) = 0;
};

#endif /* INCOMINGENTRYRECEIVER_H_ */
