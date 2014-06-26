/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/Semaphore.hpp"
#include "Base.h"

typedef void (*TimerInterruptHandler)(void *param);

void Wait(double seconds);
double GetClock();
double GetTime();

namespace wpilib { namespace internal {
    extern double simTime;
	extern MULTIWAIT_ID time_wait;
    // transport::SubscriberPtr time_sub;
}}

/**
 * Timer objects measure accumulated time in seconds.
 * The timer object functions like a stopwatch. It can be started, stopped, and cleared. When the
 * timer is running its value counts up in seconds. When stopped, the timer holds the current
 * value. The implementation simply records the time when started and subtracts the current time
 * whenever the value is requested.
 */
class Timer
{
public:
	Timer();
	virtual ~Timer();
	double Get();
	void Reset();
	void Start();
	void Stop();
	bool HasPeriodPassed(double period);

	static double GetFPGATimestamp();
	static double GetPPCTimestamp();

private:
	double m_startTime;
	double m_accumulatedTime;
	bool m_running;
	MUTEX_ID m_semaphore;
	DISALLOW_COPY_AND_ASSIGN(Timer);
};
