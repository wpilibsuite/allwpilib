/*
 * SocketServerStreamProvider.h
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#ifndef SOCKETSERVERSTREAMPROVIDER_H_
#define SOCKETSERVERSTREAMPROVIDER_H_


class SocketServerStreamProvider;

#include "networktables2/stream/IOStream.h"
#include "networktables2/stream/IOStreamProvider.h"




class SocketServerStreamProvider : public IOStreamProvider{
private:
	int serverSocket;
public:
	SocketServerStreamProvider(int port);
	virtual ~SocketServerStreamProvider();
	IOStream* accept();
	void close();
};




#endif /* SOCKETSERVERSTREAMPROVIDER_H_ */
