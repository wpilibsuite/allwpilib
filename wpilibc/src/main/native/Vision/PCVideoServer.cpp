/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"

#include "PCVideoServer.h"

#include <errno.h>
#include <taskLib.h>
#include <hostLib.h>
#include <inetLib.h>
#include <sockLib.h>
#include <cstring>

#include "NetworkCommunication/UsageReporting.h"
#include "Task.h"
#include "Timer.h"
#include "Vision/AxisCamera.h"
#include "WPIErrors.h"

/**
 * @brief Implements an object that automatically does a close on a
 * camera socket on destruction.
 */
class ScopedSocket {
public:
	ScopedSocket(int camSock)
		: m_camSock(camSock)
	{
	}

	~ScopedSocket() {
		if (m_camSock != ERROR) {
			close(m_camSock);
		}
	}
	// Cast to int allows you to pass this to any function that
	// takes the socket as an int.
	operator int() const {
		return m_camSock;
	}

private:
	int m_camSock;
};

//============================================================================
// PCVideoServer
//============================================================================

/**
 * @brief Constructor.
 */
PCVideoServer::PCVideoServer()
	: m_serverTask("PCVideoServer", (FUNCPTR)s_ServerTask)
	, m_newImageSem (NULL)
	, m_stopServer (false)
{
	AxisCamera &cam = AxisCamera::GetInstance();
	m_newImageSem = cam.GetNewImageSem();
	if (!cam.StatusIsFatal())
	{
		StartServerTask();
	}
	else
	{
		CloneError(&cam);
	}
}

/**
 * @brief Destructor.
 * Stop serving images and destroy this class.
 */
PCVideoServer::~PCVideoServer()
{
	// Stop the images to PC server.
	Stop();
	// Clear the error so that you can use this object to make a connection to
	// the VIDEO_TO_PC_PORT to stop the ImageToPCServer if it is waiting to
	// accept connections from a PC.
	ClearError();
	// Open a socket.
	int camSock = socket(AF_INET, SOCK_STREAM, 0);
	if (camSock == ERROR)
	{
		wpi_setErrnoError();
		return;
	}
	ScopedSocket scopedCamSock(camSock);
	// If successful
	if (!StatusIsFatal())
	{
		//  Create a connection to the localhost.
		struct sockaddr_in selfAddr;
		int sockAddrSize = sizeof(selfAddr);
		bzero ((char *) &selfAddr, sockAddrSize);
		selfAddr.sin_family = AF_INET;
		selfAddr.sin_len = (u_char) sockAddrSize;
		selfAddr.sin_port = htons (VIDEO_TO_PC_PORT);

		if (( (int)(selfAddr.sin_addr.s_addr = inet_addr (const_cast<char*>("localhost")) ) != ERROR) ||
			( (int)(selfAddr.sin_addr.s_addr = hostGetByName (const_cast<char*>("localhost")) ) != ERROR))
		{
			struct timeval connectTimeout;
			connectTimeout.tv_sec = 1;
			connectTimeout.tv_usec = 0;
			connectWithTimeout(camSock, (struct sockaddr *) &selfAddr, sockAddrSize, &connectTimeout);
		}
	}
}

/**
 * Start the task that is responsible for sending images to the PC.
 */
int PCVideoServer::StartServerTask()
{
	if (StatusIsFatal())
	{
		return -1;
	}
	int id = 0;
	m_stopServer = false;
	// Check for prior copy of running task
	int oldId = taskNameToId((char*)m_serverTask.GetName());
	if(oldId != ERROR)
	{
		// TODO: Report error. You are in a bad state.
		taskDelete(oldId);
	}

	// spawn video server task
	// this is done to ensure that the task is spawned with the
	// floating point context save parameter.
	bool started = m_serverTask.Start((int)this);

	id = m_serverTask.GetID();

	if (!started)
	{
		wpi_setWPIError(TaskError);
		return id;
	}
	delayTicks(1);
	return id;
}

/**
 * @brief Start sending images to the PC.
 */
void PCVideoServer::Start()
{
	StartServerTask();
}

/**
 * @brief Stop sending images to the PC.
 */
void PCVideoServer::Stop()
{
	m_stopServer = true;
}

/**
 * Static stub for kicking off the server task
 */
int PCVideoServer::s_ServerTask(PCVideoServer *thisPtr)
{
	return thisPtr->ServerTask();
}

/**
 * @brief Initialize the socket and serve images to the PC.
 * This is the task that serves images to the PC in a loop. This runs
 * as a separate task.
 */
int PCVideoServer::ServerTask()
{
	/* Setup to PC sockets */
	struct sockaddr_in serverAddr;
	int sockAddrSize = sizeof(serverAddr);
	int pcSock = ERROR;
	bzero ((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (VIDEO_TO_PC_PORT);
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	int success;
	while (true)
	{
		taskSafe();
		// Create the socket.
		if ((pcSock = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
		{
			wpi_setErrnoErrorWithContext("Failed to create the PCVideoServer socket");
			continue;
		}
		// Set the TCP socket so that it can be reused if it is in the wait state.
		int reuseAddr = 1;
		setsockopt(pcSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
		// Bind socket to local address.
		if (bind (pcSock, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
		{
			wpi_setErrnoErrorWithContext("Failed to bind the PCVideoServer port");
			close (pcSock);
			continue;
		}
		// Create queue for client connection requests.
		if (listen (pcSock, 1) == ERROR)
		{
			wpi_setErrnoErrorWithContext("Failed to listen on the PCVideoServer port");
			close (pcSock);
			continue;
		}

		struct sockaddr_in clientAddr;
		int clientAddrSize;
		int newPCSock = accept (pcSock, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
		if (newPCSock  == ERROR)
		{
			close(pcSock);
			continue;
		}
		//TODO: check camera error

		// Report usage when there is actually a connection.
		nUsageReporting::report(nUsageReporting::kResourceType_PCVideoServer, 0);

		int numBytes = 0;
		int imageDataSize = 0;
		char* imageData = NULL;

		while(!m_stopServer)
		{
			success = takeSemaphore(m_newImageSem, 1000);
			if (success == ERROR)
			{
				// If the semTake timed out, there are no new images from the camera.
				continue;
			}
			success = AxisCamera::GetInstance().CopyJPEG(&imageData, numBytes, imageDataSize);
			if (!success)
			{
				// No point in running too fast -
				Wait(1.0);
				// If camera is not initialzed you will get failure and
				// the timestamp is invalid. Reset this value and try again.
				continue;
			}

			// Write header to PC
			static const char header[4]={1,0,0,0};
			int headerSend = write(newPCSock, const_cast<char*>(header), 4);

			// Write image length to PC
			int lengthSend = write(newPCSock, reinterpret_cast<char*>(&numBytes), 4);

			// Write image to PC
			int sent = write (newPCSock, imageData, numBytes);

			// The PC probably closed connection. Get out of here
			// and try listening again.
			if (headerSend == ERROR || lengthSend == ERROR || sent == ERROR)
			{
				break;
			}
		}
		// Clean up
		delete [] imageData;
		close (newPCSock);
		newPCSock = ERROR;
		close (pcSock);
		pcSock = ERROR;
		taskUnsafe();
		Wait(0.1);
	}
	return (OK);
}

