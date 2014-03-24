/*
 * SocketStreams.h
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#ifndef SOCKETSTREAMS_H_
#define SOCKETSTREAMS_H_


class SocketStreams;


#include "networktables2/stream/IOStreamFactory.h"
#include "networktables2/stream/IOStreamProvider.h"



/**
 * Static factory for socket stream factories and providers
 * 
 * @author Mitchell
 *
 */
class SocketStreams {
public:
	/**
	 * Create a new IOStream factory 
	 * @param host
	 * @param port
	 * @return a IOStreamFactory that will create Socket Connections on the given host and port
	 * @throws IOException
	 */
	static IOStreamFactory& newStreamFactory(const char* host, int port);

	/**
	 * Create a new IOStream provider
	 * @param port
	 * @return an IOStreamProvider for a socket server on the given port
	 * @throws IOException
	 */
	static IOStreamProvider& newStreamProvider(int port);
};



#endif /* SOCKETSTREAMS_H_ */
