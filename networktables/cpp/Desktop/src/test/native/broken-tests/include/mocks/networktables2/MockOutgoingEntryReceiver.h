/*
 * MockOutgoingEntryReciever.h
 *
 *  Created on: Sep 25, 2012
 *      Author: Fredric Silberberg
 */

#ifndef MOCKOUTGOINGENTRYRECEIVER_H_
#define MOCKOUTGOINGENTRYRECEIVER_H_

#include "gmock/gmock.h"
#include "networktables2/OutgoingEntryReceiver.h"
#include "networktables2/NetworkTableEntry.h"

class MockOutgoingEntryReceiver : public OutgoingEntryReceiver {
public:
	MOCK_METHOD0(offerOutgoingAssignment, void(NetworkTableEntry& entry));
	MOCK_METHOD1(offerOutgoingUpdate, void(NetworkTableEntry& entry));
};

#endif /* MOCKOUTGOINGENTRYRECEIVER_H_ */
