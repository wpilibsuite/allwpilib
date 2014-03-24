#include "stdafx.h"
/*
 * FDIOStream.cpp
 *
 *  Created on: Sep 27, 2012
 */

#include "networktables2/stream/FDIOStream.h"
#include "networktables2/util/IOException.h"
#include "networktables2/util/EOFException.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <wininet.h>
#include <ws2tcpip.h>


FDIOStream::FDIOStream(int _fd){
  fd = _fd;
}

FDIOStream::~FDIOStream(){
	close();
}

int FDIOStream::read(void* ptr, int numbytes){
	if(numbytes==0)
		return 0;
	char* bufferPointer = (char*)ptr;
	int totalRead = 0;
	while (totalRead < numbytes) 
	{
		int numRead=recv(fd, bufferPointer, numbytes-totalRead, 0);
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
	return totalRead;
}

int Send( int sockfd,char* Data, size_t sizeData )
{
	assert(sockfd!=INVALID_SOCKET);
	bool Result_ = true;

	WSABUF wsaBuf_;
	wsaBuf_.buf = Data;
	wsaBuf_.len = (ULONG) sizeData;
	DWORD BytesSent_;

	while (WSASend( sockfd, &wsaBuf_, 1, &BytesSent_, 0, NULL, NULL ) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Result_ = false;
			break;
		}
		Sleep(1);
	}
	if (!Result_)
	{
		char Buffer[128];
		sprintf(Buffer,"Send() failed: WSA error=%d\n",WSAGetLastError());
		OutputDebugStringA(Buffer);
	}

	return(int)BytesSent_;
}

int FDIOStream::write(const void* ptr, int numbytes)
{
  int numWrote = Send(fd,(char *)ptr,numbytes);
  if(numWrote==numbytes)
    return numWrote;
  perror("write error: ");
  fflush(stderr);
  throw IOException("Could not write all bytes to fd stream");
	
}

void FDIOStream::flush(){
}

void FDIOStream::close()
{
	//Note: the close includes to close the socket so that connection can be deferred deleted while immediately closing the socket for a new socket to open
	if (fd != INVALID_SOCKET)
	{
		char Buffer[128];
		sprintf(Buffer,"closesocket %d\n",fd);
		OutputDebugStringA(Buffer);

		shutdown( fd, SD_BOTH );
		closesocket( fd );
		fd = (int)INVALID_SOCKET;  //pedantic, in case we cache as a member variable
		Sleep(20);  //give some time to take effect
	}
}

