/*
 * ClientConnectionListenerManager.h
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#ifndef CLIENTCONNECTIONLISTENERMANAGER_H_
#define CLIENTCONNECTIONLISTENERMANAGER_H_

/**
 * An object that manages connection listeners and fires events for other listeners
 * 
 * @author Mitchell
 *
 */
class ClientConnectionListenerManager
{
public:
	virtual ~ClientConnectionListenerManager()
	{
	}
	/**
	 * called when something is connected
	 */
	virtual void FireConnectedEvent() = 0;
	/**
	 * called when something is disconnected
	 */
	virtual void FireDisconnectedEvent() = 0;
};

#endif /* CLIENTCONNECTIONLISTENERMANAGER_H_ */
