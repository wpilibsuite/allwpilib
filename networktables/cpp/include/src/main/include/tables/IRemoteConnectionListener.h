/*
 * IRemoteConnectionListener.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef IREMOTECONNECTIONLISTENER_H_
#define IREMOTECONNECTIONLISTENER_H_


class IRemoteConnectionListener;

#include "tables/IRemote.h"



/**
 * A listener that listens for connection changes in a {@link IRemote} object
 * 
 * @author Mitchell
 *
 */
class IRemoteConnectionListener {
public:
	/**
	 * Called when an IRemote is connected
	 * @param remote the object that connected
	 */
	virtual void Connected(IRemote* remote) = 0;
	/**
	 * Called when an IRemote is disconnected
	 * @param remote the object that disconnected
	 */
	virtual void Disconnected(IRemote* remote) = 0;
};



#endif /* IREMOTECONNECTIONLISTENER_H_ */
