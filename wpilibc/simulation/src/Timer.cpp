/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Timer.h"

#include <time.h>

#include "simulation/simTime.h"
#include "Utility.h"


// Internal stuff
#include "simulation/SimFloatInput.h"
#include "simulation/MainNode.h"
namespace wpilib { namespace internal {
    double simTime = 0;
    std::condition_variable time_wait;
    std::mutex time_wait_mutex;

    void time_callback(const msgs::ConstFloat64Ptr &msg) {
        simTime = msg->data();
        time_wait.notify_all();
    }
}}

/**
 * Pause the task for a specified time.
 *
 * Pause the execution of the program for a specified period of time given in seconds.
 * Motors will continue to run at their last assigned values, and sensors will continue to
 * update. Only the task containing the wait will pause until the wait time is expired.
 *
 * @param seconds Length of time to pause, in seconds.
 */
void Wait(double seconds)
{
    if (seconds < 0.0) return;

    double start = wpilib::internal::simTime;

	std::unique_lock<std::mutex> lock(wpilib::internal::time_wait_mutex);
    while ((wpilib::internal::simTime - start) < seconds) {
      wpilib::internal::time_wait.wait(lock);
    }
}

/*
 * Return the FPGA system clock time in seconds.
 * This is deprecated and just forwards to Timer::GetFPGATimestamp().
 * @returns Robot running time in seconds.
 */
double GetClock()
{
	return Timer::GetFPGATimestamp();
}

/**
 * @brief Gives real-time clock system time with nanosecond resolution
 * @return The time, just in case you want the robot to start autonomous at 8pm on Saturday (except in simulation).
*/
double GetTime()
{
    return Timer::GetFPGATimestamp(); // The epoch starts when Gazebo starts
}

//for compatibility with msvc12--see C2864
const double Timer::kRolloverTime = (1ll << 32) / 1e6;
/**
 * Create a new timer object.
 *
 * Create a new timer object and reset the time to zero. The timer is initially not running and
 * must be started.
 */
Timer::Timer()
	: m_startTime (0.0)
	, m_accumulatedTime (0.0)
	, m_running (false)
{
	//Creates a semaphore to control access to critical regions.
	//Initially 'open'
	Reset();
}

/**
 * Get the current time from the timer. If the clock is running it is derived from
 * the current system clock the start time stored in the timer class. If the clock
 * is not running, then return the time when it was last stopped.
 *
 * @return unsigned Current time value for this timer in seconds
 */
double Timer::Get() const
{
	double result;
	double currentTime = GetFPGATimestamp();

	std::lock_guard<priority_mutex> sync(m_mutex);
	if(m_running)
	{
		// This math won't work if the timer rolled over (71 minutes after boot).
		// TODO: Check for it and compensate.
		result = (currentTime - m_startTime) + m_accumulatedTime;
	}
	else
	{
		result = m_accumulatedTime;
	}

	return result;
}

/**
 * Reset the timer by setting the time to 0.
 *
 * Make the timer startTime the current time so new requests will be relative to now
 */
void Timer::Reset()
{
	std::lock_guard<priority_mutex> sync(m_mutex);
	m_accumulatedTime = 0;
	m_startTime = GetFPGATimestamp();
}

/**
 * Start the timer running.
 * Just set the running flag to true indicating that all time requests should be
 * relative to the system clock.
 */
void Timer::Start()
{
	std::lock_guard<priority_mutex> sync(m_mutex);
	if (!m_running)
	{
		m_startTime = GetFPGATimestamp();
		m_running = true;
	}
}

/**
 * Stop the timer.
 * This computes the time as of now and clears the running flag, causing all
 * subsequent time requests to be read from the accumulated time rather than
 * looking at the system clock.
 */
void Timer::Stop()
{
	double temp = Get();

	std::lock_guard<priority_mutex> sync(m_mutex);
	if (m_running)
	{
		m_accumulatedTime = temp;
		m_running = false;
	}
}

/**
 * Check if the period specified has passed and if it has, advance the start
 * time by that period. This is useful to decide if it's time to do periodic
 * work without drifting later by the time it took to get around to checking.
 *
 * @param period The period to check for (in seconds).
 * @return If the period has passed.
 */
bool Timer::HasPeriodPassed(double period)
{
	if (Get() > period)
	{
		std::lock_guard<priority_mutex> sync(m_mutex);
		// Advance the start time by the period.
		// Don't set it to the current time... we want to avoid drift.
		m_startTime += period;
		return true;
	}
	return false;
}

/*
 * Return the FPGA system clock time in seconds.
 *
 * Return the time from the FPGA hardware clock in seconds since the FPGA
 * started.
 * Rolls over after 71 minutes.
 * @returns Robot running time in seconds.
 */
double Timer::GetFPGATimestamp()
{
	// FPGA returns the timestamp in microseconds
	// Call the helper GetFPGATime() in Utility.cpp
	return wpilib::internal::simTime;
}

/*
 * Not in a match.
 */
double Timer::GetMatchTime()
{
	return Timer::GetFPGATimestamp();
}

// Internal function that reads the PPC timestamp counter.
extern "C"
{
	uint32_t niTimestamp32(void);
	uint64_t niTimestamp64(void);
}
