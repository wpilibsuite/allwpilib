/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __NTTASK_H__
#define __NTTASK_H__

#if (defined __vxworks || defined WIN32)

#include "NTErrorBase.h"
#ifdef __vxworks
#include <vxWorks.h>
#endif

/**
 * WPI task is a wrapper for the native Task object.
 * All WPILib tasks are managed by a static task manager for simplified cleanup.
 **/
class NTTask : public ErrorBase
{
public:
	static const UINT32 kDefaultPriority = 101;
	static const INT32 kInvalidTaskID = -1;

	NTTask(const char* name, FUNCPTR function, INT32 priority = kDefaultPriority, UINT32 stackSize = 20000);
	virtual ~NTTask();

	#ifdef WIN32
	bool Start(void * arg0);
	#else
	bool Start(UINT32 arg0 = 0, UINT32 arg1 = 0, UINT32 arg2 = 0, UINT32 arg3 = 0, UINT32 arg4 = 0, 
			UINT32 arg5 = 0, UINT32 arg6 = 0, UINT32 arg7 = 0, UINT32 arg8 = 0, UINT32 arg9 = 0);
	#endif

	bool Restart();
	bool Stop();

	bool IsReady();
	bool IsSuspended();

	bool Suspend();
	bool Resume();

	bool Verify();

	INT32 GetPriority();
	bool SetPriority(INT32 priority);
	const char* GetName();
	INT32 GetID();

	#ifdef WIN32
	FUNCPTR m_function;
	void * m_Arg;
	#endif
private:
	char* m_taskName;

	#ifdef WIN32
	bool StartInternal();
	HANDLE m_Handle;
	DWORD m_ID;
	#else
	FUNCPTR m_function;
	INT32 m_taskID;
	#endif

	UINT32 m_stackSize;
	INT32 m_priority;
	bool HandleError(STATUS results);
	DISALLOW_COPY_AND_ASSIGN(NTTask);
};

#endif // __vxworks
#endif // __TASK_H__
