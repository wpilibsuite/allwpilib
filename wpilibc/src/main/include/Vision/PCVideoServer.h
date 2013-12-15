/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __PC_VIDEO_SERVER_H__
#define __PC_VIDEO_SERVER_H__

#include "Task.h"
#include <semLib.h>

/** port for sending video to laptop */
#define VIDEO_TO_PC_PORT 1180

/**
 * Class the serves images to the PC.
 */
class PCVideoServer : public ErrorBase {

public:
	PCVideoServer();
	~PCVideoServer();
	unsigned int Release();
	void Start();
	void Stop();

private:
	static int s_ServerTask(PCVideoServer *thisPtr);
	int ServerTask();
	int StartServerTask();

	Task m_serverTask;
	SEMAPHORE_ID m_newImageSem;
	bool m_stopServer;
};

#endif

