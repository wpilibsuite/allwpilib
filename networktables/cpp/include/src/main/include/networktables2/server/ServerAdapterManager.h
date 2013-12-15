/*
 * ServerAdapterManager.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#ifndef SERVERADAPTERMANAGER_H_
#define SERVERADAPTERMANAGER_H_

class ServerAdapterManager;

#include "networktables2/server/ServerConnectionAdapter.h"

/**
 * A class that manages connections to a server
 * 
 * @author Mitchell
 *
 */
class ServerAdapterManager
{
public:
	virtual ~ServerAdapterManager()
	{
	}
	/**
	 * Called when a connection adapter has been closed
	 * @param connectionAdapter the adapter that was closed
	 */
	virtual void close(ServerConnectionAdapter& connectionAdapter, bool closeStream) = 0;
};

#endif /* SERVERADAPTERMANAGER_H_ */
