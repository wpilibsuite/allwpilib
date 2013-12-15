

#ifndef MOCK_SERVER_CONN__H
#define MOCK_SERVER_CONN__H
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "networktables2/server/ServerConnectionAdapter.h"


class MockServerConnectionAdapter : public ServerConnectionAdapter {
 public:
	MOCK_METHOD1(gotoState, void(ServerConnectionState* newState));
	MOCK_METHOD1(badMessage, void(BadMessageException& e));
	MOCK_METHOD1(ioException, void(IOException& e));
	MOCK_METHOD1(shutdown, void(bool closeStream));
	MOCK_METHOD1(clientHello, void(uint16_t protocolRevision));
	MOCK_METHOD1(protocolVersionUnsupported, void(uint16_t protocolRevision));
	MOCK_METHOD0(keepAlive, void());
	MOCK_METHOD0(serverHelloComplete, void());
	MOCK_METHOD0(flush, void());
	MOCK_METHOD0(ensureAlive, void());
	MOCK_METHOD0(getConnectionState, ServerConnectionState*());
	MOCK_METHOD1(offerIncomingAssignment, void(NetworkTableEntry* entry));
	MOCK_METHOD1(offerOutgoingAssignment, void(NetworkTableEntry* entry));
	MOCK_METHOD1(offerOutgoingUpdate, void(NetworkTableEntry* entry));
	MOCK_METHOD3(offerIncomingUpdate, void(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value));
	MOCK_METHOD1(getEntry, NetworkTableEntry*(EntryId id));
	MOCK_METHOD0(stop, void());
	MOCK_METHOD0(isRunning, bool());
};



#endif /* IOSTREAMPROVIDERMOCK_H_ */
