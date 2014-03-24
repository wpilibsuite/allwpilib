#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "mocks/networktables2/NetworkTableEntry.h"
#include "mocks/networktables2/MockServerConnectionAdapter.h"
#include "networktables2/server/ServerConnectionList.h"

using namespace testing;


TEST(ConnectionListTests, testSendTransaction) {
	MockServerConnectionAdapter* adapter1 = new MockServerConnectionAdapter();
	MockServerConnectionAdapter* adapter2 = new MockServerConnectionAdapter();
	MockNetworkTableEntry* entry1 = new MockNetworkTableEntry();
	MockNetworkTableEntry* entry2 = new MockNetworkTableEntry();

	ServerConnectionList* connectionList = new ServerConnectionList();
	connectionList->add(adapter1);
	connectionList->add(adapter2);

	context.checking(new Expectations() {{
		oneOf(adapter1).offerOutgoingAssignment(entry1);
		oneOf(adapter2).offerOutgoingAssignment(entry1);
	}});
	EXPECT_CALL(*adapter1, offerOutgoingAssignment(Pointee(entry1))
			.Times(AtLeast(1)));
	EXPECT_CALL(*adapter2, offerOutgoingAssignment(Pointee(entry1))
					.Times(AtLeast(1)));
//	EXPECT_CALL(*streamProvider, close())
//		.Times(1);
//	EXPECT_CALL(*thread1, stop())
//		.Times(1);
//	EXPECT_CALL(*thread2, stop())
//		.Times(1);
//	NetworkTableServer *server = new NetworkTableServer(*streamProvider, *(new NetworkTableEntryTypeManager()), *threadManager);
//	server->close();
//	delete threadManager;
//	delete streamProvider;
//	delete thread1;
//	delete thread2;
}

