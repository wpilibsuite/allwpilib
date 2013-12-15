/*
 * ServerConnectionList.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/server/ServerConnectionList.h"
#include "networktables2/server/ServerIncomingStreamMonitor.h"
#include <algorithm>
#include <stdio.h>



ServerConnectionList::ServerConnectionList(ServerIncomingStreamMonitor *Factory) : m_Factory(Factory)
{
}
ServerConnectionList::~ServerConnectionList()
{
	connectionsLock.take();
	closeAll();
}

void ServerConnectionList::add(ServerConnectionAdapter& connection)
{ 
	NTSynchronized sync(connectionsLock);
	connections.push_back(&connection);
}

void ServerConnectionList::close(ServerConnectionAdapter& connectionAdapter, bool closeStream)
{ 
	NTSynchronized sync(connectionsLock);
	std::vector<ServerConnectionAdapter*>::iterator connectionPosition = std::find(connections.begin(), connections.end(), &connectionAdapter);
	if (connectionPosition != connections.end() && (*connectionPosition)==&connectionAdapter)
	{
	        fprintf(stdout, "[NT] Close: %p\n", (void*)&connectionAdapter);
		fflush(stdout);
		connections.erase(connectionPosition);
		m_Factory->close(&connectionAdapter);
		//connectionAdapter.shutdown(closeStream);
		//delete &connectionAdapter;
	}
}

void ServerConnectionList::closeAll()
{ 
	NTSynchronized sync(connectionsLock);
	while(connections.size() > 0)
	{
		close(*connections[0], true);
	}
}

void ServerConnectionList::offerOutgoingAssignment(NetworkTableEntry* entry)
{ 
	NTSynchronized sync(connectionsLock);
	for(unsigned int i = 0; i < connections.size(); ++i)
	{
		connections[i]->offerOutgoingAssignment(entry);
	}
}
void ServerConnectionList::offerOutgoingUpdate(NetworkTableEntry* entry)
{ 
	NTSynchronized sync(connectionsLock);
	for(unsigned int i = 0; i < connections.size(); ++i)
	{
		connections[i]->offerOutgoingUpdate(entry);
	}
}
void ServerConnectionList::flush()
{ 
	NTSynchronized sync(connectionsLock);
	for(unsigned int i = 0; i < connections.size(); ++i)
	{
		connections[i]->flush();
	}
}
void ServerConnectionList::ensureAlive()
{ 
	NTSynchronized sync(connectionsLock);
	for(unsigned int i = 0; i < connections.size(); ++i)
	{
		connections[i]->ensureAlive();
	}
}
