

#ifndef IOSTREAMPROVIDERMOCK_H_
#define IOSTREAMPROVIDERMOCK_H_
#include "networktables2/stream/IOStreamProvider.h"

class MockIOStreamProvider : public IOStreamProvider
{
public:
	MOCK_METHOD0(accept, IOStream*());
	MOCK_METHOD0(close, void());
};

#endif /* IOSTREAMPROVIDERMOCK_H_ */
