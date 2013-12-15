/*
 * SocketStreamFactory.h
 *
 *  Created on: Nov 3, 2012
 *      Author: Mitchell Wills
 */

#ifndef SOCKETSTREAMFACTORY_H_
#define SOCKETSTREAMFACTORY_H_

class SocketStreamFactory;

#include "networktables2/stream/IOStreamFactory.h"
#include <string>

/**
 * 
 * @author mwills
 *
 */
class SocketStreamFactory : public IOStreamFactory{
private:
	const char* host;
	const int port;

public:
	SocketStreamFactory(const char* host, int port);
	virtual ~SocketStreamFactory();

	IOStream* createStream();

};



#endif /* SOCKETSTREAMFACTORY_H_ */
