/*
 * OutgoingEntryReceiver.h
 *
 *  Created on: Sep 19, 2012
 *      Author: Mitchell Wills
 */

#ifndef OUTGOINGENTRYRECEIVER_H_
#define OUTGOINGENTRYRECEIVER_H_

class OutgoingEntryReceiver;

#include "networktables2/NetworkTableEntry.h"

class NetworkTableEntry;

class OutgoingEntryReceiver
{
public:
	virtual ~OutgoingEntryReceiver()
	{
	}
	virtual void offerOutgoingAssignment(NetworkTableEntry* entry) = 0;
	virtual void offerOutgoingUpdate(NetworkTableEntry* entry) = 0;
};
class OutgoingEntryReceiver_NULL_t : public OutgoingEntryReceiver {
public:
	void offerOutgoingAssignment(NetworkTableEntry* entry);
	void offerOutgoingUpdate(NetworkTableEntry* entry);
};

extern OutgoingEntryReceiver_NULL_t OutgoingEntryReceiver_NULL;

#endif /* OUTGOINGENTRYRECEIVER_H_ */
