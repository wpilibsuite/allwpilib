/*
 * NetworkTableServer.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/server/NetworkTableServer.h"
#include "networktables2/server/ServerNetworkTableEntryStore.h"
#include <iostream>
#include <limits.h>

NetworkTableServer::NetworkTableServer(IOStreamProvider& _streamProvider, NetworkTableEntryTypeManager& typeManager, NTThreadManager& threadManager):
		NetworkTableNode(*new ServerNetworkTableEntryStore(*this)),
		streamProvider(_streamProvider),
		incomingStreamMonitor(streamProvider, (ServerNetworkTableEntryStore&)entryStore, *this, connectionList, typeManager, threadManager),
		connectionList(&incomingStreamMonitor),
		writeManager(connectionList, threadManager, GetEntryStore(), ULONG_MAX),
                continuingReceiver(writeManager){
	
	GetEntryStore().SetIncomingReceiver(&continuingReceiver);
	GetEntryStore().SetOutgoingReceiver(&continuingReceiver);
	
	incomingStreamMonitor.start();
	writeManager.start();
}
//TODO implement simplified NetworkTableServer constructor
/*NetworkTableServer::NetworkTableServer(IOStreamProvider& streamProvider){
	this(streamProvider, new NetworkTableEntryTypeManager(), new DefaultThreadManager());
}*/
NetworkTableServer::~NetworkTableServer(){
	Close();
	delete &entryStore;
}

void NetworkTableServer::Close(){
	try{
		incomingStreamMonitor.stop();
		writeManager.stop();
		connectionList.closeAll();
	} catch (const std::exception& ex) {
	    //TODO print stack trace?
	}
}

void NetworkTableServer::OnNewConnection(ServerConnectionAdapter& connectionAdapter) {
	connectionList.add(connectionAdapter);
}


bool NetworkTableServer::IsConnected() {
	return true;
}


bool NetworkTableServer::IsServer() {
	return true;
}
