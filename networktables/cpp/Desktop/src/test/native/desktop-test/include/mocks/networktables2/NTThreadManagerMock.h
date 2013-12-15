

#ifndef NTTHREADMANGRRMOCK_H_
#define NTTHREADMANGRRMOCK_H_


#include "networktables2/thread/NTThreadManager.h"

class MockNTThreadManager : public NTThreadManager
{
public:
	MOCK_METHOD2(newBlockingPeriodicThread, NTThread*(PeriodicRunnable* r, const char* name));
};



#endif /* IOSTREAMPROVIDERMOCK_H_ */
