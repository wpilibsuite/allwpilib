

#ifndef NTTHREADMOCK_H_
#define NTTHREADMOCK_H_

#include "networktables2/thread/NTThreadManager.h"

class MockNTThread : public NTThread
{
public:
	MOCK_METHOD0(stop, void());
	MOCK_METHOD0(isRunning, bool());
};

#endif /* IOSTREAMPROVIDERMOCK_H_ */
