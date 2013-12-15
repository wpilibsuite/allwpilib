/*
 * IOStream.h
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#ifndef IOSTREAM_H_
#define IOSTREAM_H_

class IOStream;

class IOStream
{
public:
	virtual ~IOStream()
	{
	}
	virtual int read(void* ptr, int numbytes) = 0;
	virtual int write(const void* ptr, int numbytes) = 0;
	virtual void flush() = 0;
	virtual void close() = 0;
};

#endif /* IOSTREAM_H_ */
