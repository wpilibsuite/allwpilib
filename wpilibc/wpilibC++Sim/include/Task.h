/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/Task.hpp"

/**
 * WPI task is a wrapper for the native Task object.
 * All WPILib tasks are managed by a static task manager for simplified cleanup.
 **/
class Task : public ErrorBase
{
public:
	static const uint32_t kDefaultPriority = 101;

	Task(const char* name, FUNCPTR function, int32_t priority = kDefaultPriority,
			uint32_t stackSize = 20000);
	virtual ~Task();

	bool Start(uint32_t arg0 = 0, uint32_t arg1 = 0, uint32_t arg2 = 0, uint32_t arg3 = 0,
			uint32_t arg4 = 0, uint32_t arg5 = 0, uint32_t arg6 = 0, uint32_t arg7 = 0,
			uint32_t arg8 = 0, uint32_t arg9 = 0);
	bool Restart();
	bool Stop();

	bool IsReady();
	bool IsSuspended();

	bool Suspend();
	bool Resume();

	bool Verify();

	int32_t GetPriority();
	bool SetPriority(int32_t priority);
	const char* GetName();
	TASK GetID();

private:
	FUNCPTR m_function;
	char* m_taskName;
	TASK m_taskID;
	uint32_t m_stackSize;
	int m_priority;
	bool HandleError(STATUS results);
	DISALLOW_COPY_AND_ASSIGN(Task);
};

