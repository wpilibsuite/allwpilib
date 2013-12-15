/*
 * NetworkTableConnectionListenerAdapter.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#include "networktables/NetworkTableConnectionListenerAdapter.h"


NetworkTableConnectionListenerAdapter::NetworkTableConnectionListenerAdapter(IRemote* _targetSource, IRemoteConnectionListener* _targetListener):
			targetSource(_targetSource), targetListener(_targetListener){}
NetworkTableConnectionListenerAdapter::~NetworkTableConnectionListenerAdapter(){}

void NetworkTableConnectionListenerAdapter::Connected(IRemote* remote) {
	targetListener->Connected(targetSource);
}

void NetworkTableConnectionListenerAdapter::Disconnected(IRemote* remote) {
	targetListener->Disconnected(targetSource);
}

