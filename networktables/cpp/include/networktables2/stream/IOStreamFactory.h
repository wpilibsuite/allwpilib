/*
 * IOStreamFactory.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef IOSTREAMFACTORY_H_
#define IOSTREAMFACTORY_H_

#include "networktables2/stream/IOStream.h"

/**
 * A factory that will create the same IOStream. A stream returned by this factory should be closed before calling createStream again
 * 
 * @author Mitchell
 *
 */
class IOStreamFactory
{
public:
	virtual ~IOStreamFactory()
	{
	}
	/**
	 * @return create a new stream
	 * @throws IOException
	 */
	virtual IOStream* createStream() = 0;
};

#endif /* IOSTREAMFACTORY_H_ */
