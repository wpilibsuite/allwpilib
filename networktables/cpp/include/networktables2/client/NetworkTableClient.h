/*
 * NetworkTableClient.h
 *
 *  Created on: Nov 3, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLECLIENT_H_
#define NETWORKTABLECLIENT_H_

class NetworkTableClient;

#include "networktables2/NetworkTableNode.h"
#include "networktables2/client/ClientConnectionAdapter.h"
#include "networktables2/WriteManager.h"
#include "networktables2/TransactionDirtier.h"

/**
 * A client node in NetworkTables 2.0
 * 
 * @author Mitchell
 *
 */
class NetworkTableClient : public NetworkTableNode{
private:
	ClientConnectionAdapter& adapter;
	WriteManager& writeManager;
        TransactionDirtier* dirtier;

public:
	/**
	 * Create a new NetworkTable Client
	 * @param streamFactory
	 * @param threadManager
	 * @param transactionPool
	 */
	NetworkTableClient(IOStreamFactory& streamFactory, NetworkTableEntryTypeManager& typeManager, NTThreadManager& threadManager);
	~NetworkTableClient();

	/**
	 * force the client to disconnect and reconnect to the server again. Will connect if the client is currently disconnected
	 */
	void reconnect();

	void Close();
	
	void stop();

	bool IsConnected();

	bool IsServer();

};


#endif /* NETWORKTABLECLIENT_H_ */
