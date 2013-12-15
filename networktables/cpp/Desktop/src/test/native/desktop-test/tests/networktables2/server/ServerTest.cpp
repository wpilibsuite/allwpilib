#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "mocks/networktables2/IOStreamProviderMock.h"
#include "mocks/networktables2/NTThreadManagerMock.h"
#include "mocks/networktables2/NTThreadMock.h"
#include "networktables2/server/NetworkTableServer.h"

using namespace testing;

TEST(ServerTest, testClose) {
        MockIOStreamProvider streamProvider;
	MockNTThreadManager threadManager;
	MockNTThread* thread1 = new MockNTThread();
	MockNTThread* thread2 = new MockNTThread();

	EXPECT_CALL(threadManager, newBlockingPeriodicThread(A<PeriodicRunnable*>(),
			AnyOf(StrEq("Server Incoming Stream Monitor Thread"), StrEq("Write Manager Thread"))))
			.Times(AtLeast(1))
			.WillOnce(Return(thread1))
			.WillOnce(Return(thread2));
	EXPECT_CALL(streamProvider, close()).Times(1);
	EXPECT_CALL(*thread1, stop()).Times(1);
	EXPECT_CALL(*thread2, stop()).Times(1);

	NetworkTableEntryTypeManager typeManager;
	NetworkTableServer* server = new NetworkTableServer(streamProvider, typeManager, threadManager);
	delete server;//will close stream provider

	//do not have to delete threads they are deleted by the server
}

