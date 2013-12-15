/*
* SocketStreamFactory.cpp
*
*  Created on: Nov 3, 2012
*      Author: Mitchell Wills
*/
#include "stdafx.h"

#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <winsock2.h>
#include "networktables2/stream/FDIOStream.h"
#include "networktables2/stream/SocketStreamFactory.h"

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


SocketStreamFactory::SocketStreamFactory(const char* _host, int _port):host(_host), port(_port)
{
	load_tcpip();
}

SocketStreamFactory::~SocketStreamFactory()
{
	unload_tcpip();
}

IOStream *SocketStreamFactory::createStream(){
	IOStream *ret=NULL;
	int sockfd = INVALID_SOCKET;
	try
	{
		struct sockaddr_in serv_addr;
		struct hostent *server;

		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sockfd < 0) throw 0;

		server = gethostbyname(host);

		if (server == NULL) 
			throw 1;

		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
		serv_addr.sin_port = htons(port);

		//We are outgoing so setup the socket options before making the connection
		//Setup for TCP_NODELAY for nice crisp response time...
		int on = 1;
		//Enable if we need high performance feedback
		//setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));
		// Set the linger options
		const LINGER linger = { 1, 0 };
		setsockopt( sockfd, SOL_SOCKET, SO_LINGER, (const char *)&linger, sizeof(linger) );

		//Note: on this last step... it can typically fail if there is no server present, so instead of issuing a throw I will handle the logic
		//in place.  This will avoid flooding debug output on a typical scenario
		//  [9/10/2013 JamesK]
		{
			char Buffer[128];
			sprintf(Buffer,"connecting %d\n",sockfd);
			OutputDebugStringA(Buffer);
		}
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) == 0)
			ret=new FDIOStream(sockfd);
		else
		{
			if (sockfd != INVALID_SOCKET)
			{
				//We must shut down socket before issuing the close to avoid zombie ports (Ask Kirk)
				shutdown( sockfd, SD_BOTH );
				closesocket( sockfd );
				sockfd = (int)INVALID_SOCKET;  //pedantic, in case we cache as a member variable
			}
		}
	}
	catch (int ErrorCode)
	{
		//Close resources if we cannot meet all the preconditions
		if (sockfd != INVALID_SOCKET)
		{
			//We must shut down socket before issuing the close to avoid zombie ports (Ask Kirk)
			shutdown( sockfd, SD_BOTH );
			closesocket( sockfd );
			sockfd = (int)INVALID_SOCKET;  //pedantic, in case we cache as a member variable
		}
		const char *ErrorMsg=NULL;
		switch (ErrorCode)
		{
		case 0:
			ErrorMsg="ERROR opening socket";
			break;
		case 1:
			ErrorMsg="ERROR, no such host";
			break;
		case 2:
			ErrorMsg="ERROR on connect";
			break;
		};
		if (ErrorMsg)
		{
			char Buffer[1024];
			sprintf(Buffer,"ErrorMsg=%s WSA error=%d\n",ErrorMsg,WSAGetLastError());
			OutputDebugStringA(Buffer);
			printf("ErrorMsg=%s WSA error=%d\n",ErrorMsg,WSAGetLastError());

			//If we get a WSANOTINITIALISED error... try to reset it
			if (WSAGetLastError()==WSANOTINITIALISED)
			{
				unload_tcpip();
				Sleep(100);  //just to be safe
				load_tcpip();
				//In my tests this actually works
			}
		}
		Sleep(1000); //avoid flooding to connect... it doesn't need to occur every 20ms
	}
	return ret;
}
