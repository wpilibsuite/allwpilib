/*
 * IOStreamProvider.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef IOSTREAMPROVIDER_H_
#define IOSTREAMPROVIDER_H_

#include "networktables2/stream/IOStream.h"

/**
 * An object that will provide the IOStream of clients to a NetworkTable Server
 * 
 * @author mwills
 *
 */
class IOStreamProvider
{
public:
	virtual ~IOStreamProvider()
	{
	}
	/**
	 * 
	 * @return a new IOStream normally from a server
	 * @throws IOException
	 */
	virtual IOStream* accept() = 0;
	/**
	 * Close the source of the IOStreams. {@link #accept()} should not be called after this is called
	 * @throws IOException
	 */
	virtual void close() = 0;
};

#endif /* IOSTREAMPROVIDER_H_ */
