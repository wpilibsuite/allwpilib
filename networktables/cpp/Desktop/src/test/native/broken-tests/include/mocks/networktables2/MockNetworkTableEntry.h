/*
 * MockNetworkTableEntry.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Fred
 */

#ifndef MOCKNETWORKTABLEENTRY_H_
#define MOCKNETWORKTABLEENTRY_H_

#include "networktables2/NetworkTableEntry.h"
#include "gmock/gmock.h"
using namespace std;

class MockNetworkTableEntry : public NeworkTableEntry {
public:
	MOCK_METHOD0(isDirty, void());
	MOCK_METHOD1(makeDirty, void());
};


#endif /* MOCKNETWORKTABLEENTRY_H_ */
