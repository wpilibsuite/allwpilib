/*
 * NetworkTableMode.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Mitchell Wills
 */

#include <string>
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/NetworkTableNode.h"
#include "networktables2/server/NetworkTableServer.h"
#include "networktables2/client/NetworkTableClient.h"
#include "networktables2/stream/SocketServerStreamProvider.h"
#include "networktables2/stream/SocketStreamFactory.h"
#include "networktables/NetworkTableMode.h"


NetworkTableServerMode NetworkTableMode::Server;
NetworkTableClientMode NetworkTableMode::Client;

NetworkTableServerMode::NetworkTableServerMode(){}
NetworkTableClientMode::NetworkTableClientMode(){}

static void deleteIOStreamProvider(void* ptr){
  delete (IOStreamProvider*)ptr;
}
static void deleteIOStreamFactory(void* ptr){
  delete (IOStreamFactory*)ptr;
}

NetworkTableNode* NetworkTableServerMode::CreateNode(const char* ipAddress, int port, NTThreadManager& threadManager, void*& streamFactory_out, StreamDeleter& streamDeleter_out, NetworkTableEntryTypeManager*& typeManager_out){
	IOStreamProvider* streamProvider = new SocketServerStreamProvider(port);
	streamFactory_out = streamProvider;
	typeManager_out = new NetworkTableEntryTypeManager();
	streamDeleter_out = deleteIOStreamFactory;
	return new NetworkTableServer(*streamProvider, *typeManager_out, threadManager);
}
NetworkTableNode* NetworkTableClientMode::CreateNode(const char* ipAddress, int port, NTThreadManager& threadManager, void*& streamFactory_out, StreamDeleter& streamDeleter_out, NetworkTableEntryTypeManager*& typeManager_out){
	IOStreamFactory* streamFactory = new SocketStreamFactory(ipAddress, port);
	streamFactory_out = streamFactory;
	typeManager_out = new NetworkTableEntryTypeManager();
	streamDeleter_out = deleteIOStreamProvider;
	return new NetworkTableClient(*streamFactory, *typeManager_out, threadManager);
}
