/*
 * SocketServerStreamProvider.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/stream/SocketServerStreamProvider.h"
#include "networktables2/stream/FDIOStream.h"
#include "networktables2/util/IOException.h"

#include <strings.h>
#include <cstring>
#include <errno.h>
#ifdef _WRS_KERNEL
#include <inetLib.h>
#include <selectLib.h>
#include <sockLib.h>
#include <taskLib.h>
#include <usrLib.h>
#include <ioLib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <wininet.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#endif

#ifndef _WRS_KERNEL
#define ERROR -1
#endif

#if defined(WIN32) || defined(_WRS_KERNEL)
typedef int addrlen_t;
#else
typedef socklen_t addrlen_t;
#endif


SocketServerStreamProvider::SocketServerStreamProvider(int port){
	struct sockaddr_in serverAddr;
	int sockAddrSize = sizeof(serverAddr);
	memset(&serverAddr, 0, sockAddrSize);

#ifdef _WRS_KERNEL
	serverAddr.sin_len = (u_char)sockAddrSize;
#endif
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		throw IOException("Error creating server socket", errno);
	}

	// Set the TCP socket so that it can be reused if it is in the wait state.
	int reuseAddr = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof(reuseAddr));

	// Bind socket to local address.
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sockAddrSize) == ERROR)
	{
		::close(serverSocket);
		throw IOException("Could not bind server socket", errno);
	}

	if (listen(serverSocket, 1) == ERROR)
	{
		::close(serverSocket);
		throw IOException("Could not listen on server socket", errno);
	}
}
SocketServerStreamProvider::~SocketServerStreamProvider(){
	close();
}


IOStream* SocketServerStreamProvider::accept(){
	struct timeval timeout;
	// Check for a shutdown once per second
	while (true)
	{
		fd_set fdSet;

		FD_ZERO(&fdSet);
		FD_SET(serverSocket, &fdSet);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int select_result = select(FD_SETSIZE, &fdSet, NULL, NULL, &timeout);
		if ( select_result < 0)
		  return NULL;

		if (FD_ISSET(serverSocket, &fdSet))
		  {
		    struct sockaddr clientAddr;
		    memset(&clientAddr, 0, sizeof(struct sockaddr));
		    addrlen_t clientAddrSize = sizeof(clientAddr);
		    int connectedSocket = ::accept(serverSocket, &clientAddr, &clientAddrSize);
		    if (connectedSocket == ERROR)
		      return NULL;
				
		    return new FDIOStream(connectedSocket);
		  }

		
	}
	return NULL;
}

void SocketServerStreamProvider::close(){
	::close(serverSocket);
}
