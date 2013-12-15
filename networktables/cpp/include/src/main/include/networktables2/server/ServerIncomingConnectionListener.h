/*
 * ServerIncomingConnectionListener.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#ifndef SERVERINCOMINGCONNECTIONLISTENER_H_
#define SERVERINCOMINGCONNECTIONLISTENER_H_

#include "networktables2/server/ServerConnectionAdapter.h"

/**
 * Listener for new incoming server connections
 * @author Mitchell
 *
 */
class ServerIncomingConnectionListener
{
public:
	virtual ~ServerIncomingConnectionListener()
	{
	}
	/**
	 * 
	 * Called on create of a new connection
	 * @param connectionAdapter the server connection adapter
	 */
	virtual void OnNewConnection(ServerConnectionAdapter& connectionAdapter) = 0;
};

#endif /* SERVERINCOMINGCONNECTIONLISTENER_H_ */
