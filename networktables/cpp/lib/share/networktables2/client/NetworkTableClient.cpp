/*
 * NetworkTableClient.cpp
 *
 *  Created on: Nov 3, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/client/NetworkTableClient.h"

/**
 * Create a new NetworkTable Client
 * @param streamFactory
 * @param threadManager
 * @param transactionPool
 */
NetworkTableClient::NetworkTableClient(IOStreamFactory& streamFactory, NetworkTableEntryTypeManager& typeManager, NTThreadManager& threadManager):
	NetworkTableNode(*new ClientNetworkTableEntryStore(*this)),
	adapter(*new ClientConnectionAdapter((ClientNetworkTableEntryStore&)entryStore, threadManager, streamFactory, *this, typeManager)),
	writeManager(*new WriteManager(adapter, threadManager, GetEntryStore(), 1000)),
	dirtier(new TransactionDirtier(writeManager)){
	
	GetEntryStore().SetOutgoingReceiver(dirtier);
	GetEntryStore().SetIncomingReceiver(&OutgoingEntryReceiver_NULL);
	writeManager.start();
}
NetworkTableClient::~NetworkTableClient(){
	//Closing this now will cause a reconnect from the write manager -James
	//Close();
	delete &writeManager;
	delete &adapter;
	delete &entryStore;
	delete dirtier;
}

/**
 * force the client to disconnect and reconnect to the server again. Will connect if the client is currently disconnected
 */
void NetworkTableClient::reconnect() {
	adapter.reconnect();
}

void NetworkTableClient::Close() {
	adapter.close();
}

void NetworkTableClient::stop() {
	writeManager.stop();
	Close();
}

bool NetworkTableClient::IsConnected() {
	return adapter.isConnected();
}

bool NetworkTableClient::IsServer() {
	return false;
}
