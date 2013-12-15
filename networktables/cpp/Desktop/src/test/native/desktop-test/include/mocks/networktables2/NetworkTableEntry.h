

#ifndef IOSTREAMPROVIDERMOCK_H_
#define IOSTREAMPROVIDERMOCK_H_
#include "networktables2/NetworkTableEntry.h"

class MockNetworkTableEntry : public NetworkTableEntry
{
public:
	MOCK_METHOD0(makeDirty, void());
	MOCK_METHOD0(makeClean, void());
	MOCK_METHOD0(isDirty, bool());
	MOCK_METHOD0(clearId, void());
	MOCK_METHOD0(getId, EntryId());
	MOCK_METHOD0(getValue, EntryValue());
	MOCK_METHOD0(getType, NetworkTableEntryType*());
	MOCK_METHOD0(getSequenceNumber, SequenceNumber());
	MOCK_METHOD3(forcePut, void(SequenceNumber newSequenceNumber, NetworkTableEntryType* type, EntryValue newValue));
	MOCK_METHOD2(putValue, bool(SequenceNumber newSequenceNumber, EntryValue newValue));
	MOCK_METHOD2(forcePut, void(SequenceNumber newSequenceNumber, EntryValue newValue));


	MOCK_METHOD1(sendValue, void(DataIOStream& iostream));
	MOCK_METHOD1(setId, void(EntryId id));
	MOCK_METHOD1(send, void(NetworkTableConnection& connection));
	MOCK_METHOD1(fireListener, void(TableListenerManager& listenerManager));

};

#endif /* IOSTREAMPROVIDERMOCK_H_ */
