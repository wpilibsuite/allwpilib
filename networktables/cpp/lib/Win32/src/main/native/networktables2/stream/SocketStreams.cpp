/*
 * SocketStreams.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/stream/SocketStreams.h"
#include "networktables2/stream/SocketStreamFactory.h"
#include "networktables2/stream/SocketServerStreamProvider.h"



IOStreamFactory& SocketStreams::newStreamFactory(const char* host, int port){
	return *new SocketStreamFactory(host, port);
}

IOStreamProvider& SocketStreams::newStreamProvider(int port){
	return *new SocketServerStreamProvider(port);
}

