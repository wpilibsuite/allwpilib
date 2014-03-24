/*
 * IRemote.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef IREMOTE_H_
#define IREMOTE_H_

class IRemote;


#include "tables/IRemoteConnectionListener.h"



/**
 * Represents an object that has a remote connection
 * 
 * @author Mitchell
 *
 */
class IRemote {
public:
	/**
	 * Register an object to listen for connection and disconnection events
	 * 
	 * @param listener the listener to be register
	 * @param immediateNotify if the listener object should be notified of the current connection state
	 */
	virtual void AddConnectionListener(IRemoteConnectionListener* listener, bool immediateNotify) = 0;

	/**
	 * Unregister a listener from connection events
	 * 
	 * @param listener the listener to be unregistered
	 */
	virtual void RemoveConnectionListener(IRemoteConnectionListener* listener) = 0;
	
	/**
	 * Get the current state of the objects connection
	 * @return the current connection state
	 */
	virtual bool IsConnected() = 0;
	
	/**
	 * If the object is acting as a server
	 * @return if the object is a server
	 */
	virtual bool IsServer() = 0;
};


#endif /* IREMOTE_H_ */
