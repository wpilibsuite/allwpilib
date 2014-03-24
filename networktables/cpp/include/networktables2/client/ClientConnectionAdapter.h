/*
 * ClientConnectionAdapter.h
 *
 *  Created on: Nov 2, 2012
 *      Author: Mitchell Wills
 */

#ifndef CLIENTCONNECTIONADAPTER_H_
#define CLIENTCONNECTIONADAPTER_H_

class ClientConnectionAdapter;

#include "networktables2/connection/ConnectionAdapter.h"
#include "networktables2/IncomingEntryReceiver.h"
#include "networktables2/FlushableOutgoingEntryReceiver.h"
#include "networktables2/client/ClientNetworkTableEntryStore.h"
#include "networktables2/stream/IOStreamFactory.h"
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/thread/NTThread.h"
#include "networktables2/client/ClientConnectionState.h"
#include "networktables2/client/ClientConnectionListenerManager.h"
#include "networktables2/connection/ConnectionMonitorThread.h"


/**
 * Object that adapts messages from a server
 * 
 * @author Mitchell
 *
 */
class ClientConnectionAdapter : public ConnectionAdapter, public IncomingEntryReceiver, public FlushableOutgoingEntryReceiver{
private:
	ClientNetworkTableEntryStore& entryStore;
	IOStreamFactory& streamFactory;
	NTThreadManager& threadManager;
	
	ClientConnectionState* connectionState;
	ClientConnectionListenerManager& connectionListenerManager;
	NTReentrantSemaphore LOCK;
	NetworkTableEntryTypeManager& typeManager;
	NTThread* readThread;
	ConnectionMonitorThread* monitor;
	NetworkTableConnection* connection;

	void gotoState(ClientConnectionState* newState);
	bool m_IsConnectionClosed;  //Keep track of when this is closed to issue reconnect
public:
	/**
	 * @return the state of the connection
	 */
	ClientConnectionState* getConnectionState();
	/**
	 * @return if the client is connected to the server
	 */
	bool isConnected();

	/**
	 * Create a new ClientConnectionAdapter
	 * @param entryStore
	 * @param threadManager
	 * @param streamFactory
	 * @param transactionPool
	 * @param connectionListenerManager
	 */
	ClientConnectionAdapter(ClientNetworkTableEntryStore& entryStore, NTThreadManager& threadManager, IOStreamFactory& streamFactory, ClientConnectionListenerManager& connectionListenerManager, NetworkTableEntryTypeManager& typeManager);
	virtual ~ClientConnectionAdapter();
	
	
	/*
	 * Connection management
	 */
	/**
	 * Reconnect the client to the server (even if the client is not currently connected)
	 */
	void reconnect();
	
	/**
	 * Close the client connection
	 */
	void close();
	/**
	 * Close the connection to the server and enter the given state
	 * @param newState
	 */
	void close(ClientConnectionState* newState);
	
	

	void badMessage(BadMessageException& e);

	void ioException(IOException& e);
	
	NetworkTableEntry* GetEntry(EntryId id);
	
	
	bool keepAlive();

	void clientHello(ProtocolVersion protocolRevision);

	void protocolVersionUnsupported(ProtocolVersion protocolRevision);

	void serverHelloComplete();


	void offerIncomingAssignment(NetworkTableEntry* entry);
	void offerIncomingUpdate(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value);

	void offerOutgoingAssignment(NetworkTableEntry* entry);

	void offerOutgoingUpdate(NetworkTableEntry* entry);
	void flush();
	void ensureAlive();

};


#endif /* CLIENTCONNECTIONADAPTER_H_ */
