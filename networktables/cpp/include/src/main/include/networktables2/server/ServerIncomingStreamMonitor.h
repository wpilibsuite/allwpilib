/*
 * ServerIncomingStreamMonitor.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#ifndef SERVERINCOMINGSTREAMMONITOR_H_
#define SERVERINCOMINGSTREAMMONITOR_H_


class ServerIncomingStreamMonitor;


#include "networktables2/thread/PeriodicRunnable.h"
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/thread/NTThread.h"
#include "networktables2/stream/IOStreamProvider.h"
#include "networktables2/server/ServerIncomingConnectionListener.h"
#include "networktables2/server/ServerNetworkTableEntryStore.h"
#include "networktables2/server/ServerAdapterManager.h"
#include "networktables2/server/ServerConnectionAdapter.h"



/**
 * Thread that monitors for incoming connections
 * 
 * @author Mitchell
 *
 */
class ServerIncomingStreamMonitor : PeriodicRunnable{
private:
	IOStreamProvider& streamProvider;
	ServerNetworkTableEntryStore& entryStore;
	ServerIncomingConnectionListener& incomingListener;

	ServerAdapterManager& adapterListener;
	NetworkTableEntryTypeManager& typeManager;
	NTThreadManager& threadManager;
	NTThread* monitorThread;
	
	NTReentrantSemaphore BlockDeletionList;
	std::vector<ServerConnectionAdapter *> m_DeletionList;
public:
	/**
	 * Create a new incoming stream monitor
	 * @param streamProvider the stream provider to retrieve streams from
	 * @param entryStore the entry store for the server
	 * @param transactionPool transaction pool for the server
	 * @param incomingListener the listener that is notified of new connections
	 * @param adapterListener the listener that will listen to adapter events
	 * @param threadManager the thread manager used to create the incoming thread and provided to the Connection Adapters
	 */
	ServerIncomingStreamMonitor(IOStreamProvider& streamProvider, ServerNetworkTableEntryStore& entryStore,
			ServerIncomingConnectionListener& incomingListener,
			ServerAdapterManager& adapterListener,
			NetworkTableEntryTypeManager& typeManager, NTThreadManager& threadManager);
	
	~ServerIncomingStreamMonitor();
	/**
	 * Start the monitor thread
	 */
	void start();
	/**
	 * Stop the monitor thread
	 */
	void stop();
	
	void run();
	
	void close(ServerConnectionAdapter *Adapter);
	
};




#endif /* SERVERINCOMINGSTREAMMONITOR_H_ */
