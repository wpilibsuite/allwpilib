/*
 * FDIOStream.h
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#ifndef FDIOSTREAM_H_
#define FDIOSTREAM_H_



class FDIOStream;


#include "networktables2/stream/IOStream.h"
#include <stdio.h>



class FDIOStream : public IOStream{
private:
    //FILE* f;
    int fd;
public:
	FDIOStream(int fd);
	virtual ~FDIOStream();
	int read(void* ptr, int numbytes);
	int write(const void* ptr, int numbytes);
	void flush();
	void close();
};



#endif /* FDIOSTREAM_H_ */
