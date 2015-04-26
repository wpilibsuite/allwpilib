/*
 * SocketServerStreamProvider.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/stream/SocketServerStreamProvider.h"
#include "networktables2/stream/FDIOStream.h"
#include "networktables2/util/IOException.h"

#include <cstring>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <winsock2.h>
#include <wininet.h>
#include <ws2tcpip.h>

#ifndef ERROR
#define ERROR -1
#endif

typedef int addrlen_t;

static void load_tcpip(void)
{
	WSAData wsaData_;
	WORD wVersionRequested_ = MAKEWORD( 2, 2 );

	int result=WSAStartup( wVersionRequested_, &wsaData_ );
	assert(result==0);
}

static bool unload_tcpip(void)
{
	WSACleanup();
	return true;
}


SocketServerStreamProvider::SocketServerStreamProvider(int port)
{
	load_tcpip();

	struct sockaddr_in serverAddr;
	int sockAddrSize = sizeof(serverAddr);
	memset(&serverAddr, 0, sockAddrSize);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		throw IOException("Error creating server socket", errno);
	}

	// Set the TCP socket so that it can be reused if it is in the wait state.
	int reuseAddr = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof(reuseAddr));

	// Bind socket to local address.
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sockAddrSize) != 0)
	{
		close();
		throw IOException("Could not bind server socket", errno);
	}

	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		close();
		throw IOException("Could not listen on server socket", errno);
	}
}

SocketServerStreamProvider::~SocketServerStreamProvider()
{
	close();
	unload_tcpip();
}


IOStream* SocketServerStreamProvider::accept(){
	struct timeval timeout;
	// Check for a shutdown once per second
	if (serverSocket!=INVALID_SOCKET)
	{
		fd_set fdSet;

		FD_ZERO(&fdSet);
		FD_SET(serverSocket, &fdSet);
		if (select(FD_SETSIZE, &fdSet, NULL, NULL, &timeout) > 0)
		{
			if (FD_ISSET(serverSocket, &fdSet))
			{
				//For windows these must be NULL to work properly
				int connectedSocket = ::accept(serverSocket, NULL, NULL);
				if (connectedSocket == INVALID_SOCKET)
					return NULL;
				
				int on = 1;
				//Enable if we need high performance feedback
				//setsockopt(connectedSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
				
				return new FDIOStream(connectedSocket);
			}
		}
	}
	return NULL;
}

void SocketServerStreamProvider::close()
{
	if (serverSocket!=INVALID_SOCKET)
	{
		shutdown( serverSocket, SD_BOTH );
		closesocket( serverSocket );
		serverSocket = (int)INVALID_SOCKET;
	}
}
