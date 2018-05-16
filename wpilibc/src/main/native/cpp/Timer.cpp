/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Timer.h"

#include <chrono>
#include <thread>

#include <HAL/HAL.h>

#include "DriverStation.h"
#include "RobotController.h"

namespace frc {

/**
 * Pause the task for a specified time.
 *
 * Pause the execution of the program for a specified period of time given in
 * seconds. Motors will continue to run at their last assigned values, and
 * sensors will continue to update. Only the task containing the wait will pause
 * until the wait time is expired.
 *
 * @param seconds Length of time to pause, in seconds.
 */
void Wait(double seconds) {
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

/**
 * Return the FPGA system clock time in seconds.
 *
 * This is deprecated and just forwards to Timer::GetFPGATimestamp().
 *
 * @return Robot running time in seconds.
 */
double GetClock() { return Timer::GetFPGATimestamp(); }

/**
 * @brief  Gives real-time clock system time with nanosecond resolution
 * @return The time, just in case you want the robot to start autonomous at 8pm
 *         on Saturday.
 */
double GetTime() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::system_clock;

  return duration_cast<duration<double>>(system_clock::now().time_since_epoch())
      .count();
}

}  // namespace frc

using namespace frc;

// for compatibility with msvc12--see C2864
const double Timer::kRolloverTime = (1ll << 32) / 1e6;
/**
 * Create a new timer object.
 *
 * Create a new timer object and reset the time to zero. The timer is initially
 * not running and must be started.
 */
Timer::Timer() { Reset(); }

/**
 * Get the current time from the timer. If the clock is running it is derived
 * from the current system clock the start time stored in the timer class. If
 * the clock is not running, then return the time when it was last stopped.
 *
 * @return Current time value for this timer in seconds
 */
double Timer::Get() const {
  double result;
  double currentTime = GetFPGATimestamp();

  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (m_running) {
    // If the current time is before the start time, then the FPGA clock rolled
    // over. Compensate by adding the ~71 minutes that it takes to roll over to
    // the current time.
    if (currentTime < m_startTime) {
      currentTime += kRolloverTime;
    }

    result = (currentTime - m_startTime) + m_accumulatedTime;
  } else {
    result = m_accumulatedTime;
  }

  return result;
}

/**
 * Reset the timer by setting the time to 0.
 *
 * Make the timer startTime the current time so new requests will be relative to
 * now.
 */
void Timer::Reset() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_accumulatedTime = 0;
  m_startTime = GetFPGATimestamp();
}

/**
 * Start the timer running.
 *
 * Just set the running flag to true indicating that all time requests should be
 * relative to the system clock.
 */
void Timer::Start() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (!m_running) {
    m_startTime = GetFPGATimestamp();
    m_running = true;
  }
}

/**
 * Stop the timer.
 *
 * This computes the time as of now and clears the running flag, causing all
 * subsequent time requests to be read from the accumulated time rather than
 * looking at the system clock.
 */
void Timer::Stop() {
  double temp = Get();

  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (m_running) {
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
 * @return       True if the period has passed.
 */
bool Timer::HasPeriodPassed(double period) {
  if (Get() > period) {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    // Advance the start time by the period.
    m_startTime += period;
    // Don't set it to the current time... we want to avoid drift.
    return true;
  }
  return false;
}

/**
 * Return the FPGA system clock time in seconds.
 *
 * Return the time from the FPGA hardware clock in seconds since the FPGA
 * started. Rolls over after 71 minutes.
 *
 * @returns Robot running time in seconds.
 */
double Timer::GetFPGATimestamp() {
  // FPGA returns the timestamp in microseconds
  return RobotController::GetFPGATime() * 1.0e-6;
}

/**
 * Return the approximate match time.
 *
 * The FMS does not send an official match time to the robots, but does send an
 * approximate match time.  The value will count down the time remaining in the
 * current period (auto or teleop).
 *
 * Warning: This is not an official time (so it cannot be used to dispute ref
 * calls or guarantee that a function will trigger before the match ends).
 *
 * The Practice Match function of the DS approximates the behaviour seen on the
 * field.
 *
 * @return Time remaining in current match period (auto or teleop)
 */
double Timer::GetMatchTime() {
  return DriverStation::GetInstance().GetMatchTime();
}
