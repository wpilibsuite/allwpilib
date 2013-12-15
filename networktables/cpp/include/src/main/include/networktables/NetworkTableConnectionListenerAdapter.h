/*
 * NetworkTableConnectionListenerAdapter.h
 *
 *  Created on: Oct 17, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLECONNECTIONLISTENERADAPTER_H_
#define NETWORKTABLECONNECTIONLISTENERADAPTER_H_


class NetworkTableConnectionListenerAdapter;

#include "tables/IRemote.h"
#include "tables/IRemoteConnectionListener.h"


class NetworkTableConnectionListenerAdapter : public IRemoteConnectionListener{
private:
	IRemote* targetSource;
	IRemoteConnectionListener* targetListener;

public:
	NetworkTableConnectionListenerAdapter(IRemote* targetSource, IRemoteConnectionListener* targetListener);
	virtual ~NetworkTableConnectionListenerAdapter();
	void Connected(IRemote* remote);
	void Disconnected(IRemote* remote);
};


#endif /* NETWORKTABLECONNECTIONLISTENERADAPTER_H_ */
