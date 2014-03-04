/*
 * FDIOStream.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/stream/FDIOStream.h"
#include "networktables2/util/IOException.h"
#include "networktables2/util/EOFException.h"

#include <errno.h>
#include <stdlib.h>
#ifdef _WRS_KERNEL
#include <iolib.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#include <stdio.h>


FDIOStream::FDIOStream(int _fd){
  fd = _fd;
  //	f = fdopen(_fd, "rbwb");
  //	if(f==NULL)
  //		throw IOException("Could not open stream from file descriptor", errno);
  // Set the TCP socket to be non-blocking
  int flags = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
  {
    ::close(fd);
    throw IOException("Could not set socket to non-blocking mode");
  }
}
FDIOStream::~FDIOStream(){
	close();
}

int FDIOStream::read(void* ptr, int numbytes){
	if(numbytes==0)
		return 0;
	char* bufferPointer = (char*)ptr;
	int totalRead = 0;

	struct timeval timeout;
	fd_set fdSet;
	
	while (totalRead < numbytes) {
		FD_ZERO(&fdSet);
		FD_SET(fd, &fdSet);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int select_result = select(FD_SETSIZE, &fdSet, NULL, NULL, &timeout);
		if ( select_result < 0)
		  throw IOException("Select returned an error on read");

		int numRead = 0;
		if (FD_ISSET(fd, &fdSet)) {
		  numRead = ::read(fd, bufferPointer, numbytes-totalRead);

		  if(numRead == 0){
		    throw EOFException();
		  }
		  else if (numRead < 0) {
		    perror("read error: ");
		    fflush(stderr);
		    throw IOException("Error on FDIO read");
		  }
		  bufferPointer += numRead;
		  totalRead += numRead;
		}
	}
	return totalRead;
}
int FDIOStream::write(const void* ptr, int numbytes){
	int numWrote = ::write(fd, (char*)ptr, numbytes);
	if(numWrote==numbytes)
		return numWrote;

	if (numWrote == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
	{
		// see if write timeout expires
		struct timeval timeout;
		fd_set fdSet;

		FD_ZERO(&fdSet);
		FD_SET(fd, &fdSet);
		timeout.tv_sec = 1;		// wait 1 second for the other side to connect
		timeout.tv_usec = 0;

		int select_result = select(FD_SETSIZE, NULL, &fdSet, NULL, &timeout);
		if ( select_result < 0)
			throw IOException("Select returned an error on write");

		if (FD_ISSET(fd, &fdSet)) {
			numWrote = ::write(fd, (char*)ptr, numbytes);
			if(numWrote==numbytes)
				return numWrote;
		}
	}
 	
	perror("write error: ");
	fflush(stderr);
	throw IOException("Could not write all bytes to fd stream");
}
void FDIOStream::flush(){
  //if(fflush(f)==EOF)
  //  throw EOFException();
}
void FDIOStream::close(){
  //fclose(f);//ignore any errors closing
  ::close(fd);
}

