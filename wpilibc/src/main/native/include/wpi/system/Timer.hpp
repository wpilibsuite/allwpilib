// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/time.h>

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
void Wait(units::second_t seconds);

/**
 * @brief  Gives real-time clock system time with nanosecond resolution
 * @return The time, just in case you want the robot to start autonomous at 8pm
 *         on Saturday.
 */
units::second_t GetTime();

/**
 * A timer class.
 *
 * Note that if the user calls frc::sim::RestartTiming(), they should also reset
 * the timer so Get() won't return a negative duration.
 */
class Timer {
 public:
  /**
   * Create a new timer object.
   *
   * Create a new timer object and reset the time to zero. The timer is
   * initially not running and must be started.
   */
  Timer();

  virtual ~Timer() = default;

  Timer(const Timer&) = default;
  Timer& operator=(const Timer&) = default;
  Timer(Timer&&) = default;
  Timer& operator=(Timer&&) = default;

  /**
   * Get the current time from the timer. If the clock is running it is derived
   * from the current system clock the start time stored in the timer class. If
   * the clock is not running, then return the time when it was last stopped.
   *
   * @return Current time value for this timer in seconds
   */
  units::second_t Get() const;

  /**
   * Reset the timer by setting the time to 0.
   *
   * Make the timer startTime the current time so new requests will be relative
   * to now.
   */
  void Reset();

  /**
   * Start the timer running.
   *
   * Just set the running flag to true indicating that all time requests should
   * be relative to the system clock. Note that this method is a no-op if the
   * timer is already running.
   */
  void Start();

  /**
   * Restart the timer by stopping the timer, if it is not already stopped,
   * resetting the accumulated time, then starting the timer again. If you
   * want an event to periodically reoccur at some time interval from the
   * start time, consider using AdvanceIfElapsed() instead.
   */
  void Restart();

  /**
   * Stop the timer.
   *
   * This computes the time as of now and clears the running flag, causing all
   * subsequent time requests to be read from the accumulated time rather than
   * looking at the system clock.
   */
  void Stop();

  /**
   * Check if the period specified has passed.
   *
   * @param period The period to check.
   * @return       True if the period has passed.
   */
  bool HasElapsed(units::second_t period) const;

  /**
   * Check if the period specified has passed and if it has, advance the start
   * time by that period. This is useful to decide if it's time to do periodic
   * work without drifting later by the time it took to get around to checking.
   *
   * @param period The period to check for.
   * @return       True if the period has passed.
   */
  bool AdvanceIfElapsed(units::second_t period);

  /**
   * Whether the timer is currently running.
   *
   * @return true if running.
   */
  bool IsRunning() const;

  /**
   * Return the clock time in seconds. By default, the time is based on the FPGA
   * hardware clock in seconds since the FPGA started. However, the return value
   * of this method may be modified to use any time base, including
   * non-monotonic time bases.
   *
   * @returns Robot running time in seconds.
   */
  static units::second_t GetTimestamp();

  /**
   * Return the FPGA system clock time in seconds.
   *
   * Return the time from the FPGA hardware clock in seconds since the FPGA
   * started. Rolls over after 71 minutes.
   *
   * @returns Robot running time in seconds.
   */
  static units::second_t GetFPGATimestamp();

  /**
   * Return the approximate match time.
   *
   * The FMS does not send an official match time to the robots, but does send
   * an approximate match time. The value will count down the time remaining in
   * the current period (auto or teleop).
   *
   * Warning: This is not an official time (so it cannot be used to dispute ref
   * calls or guarantee that a function will trigger before the match ends).
   *
   * The Practice Match function of the DS approximates the behavior seen on the
   * field.
   *
   * @return Time remaining in current match period (auto or teleop)
   */
  static units::second_t GetMatchTime();

 private:
  units::second_t m_startTime = 0_s;
  units::second_t m_accumulatedTime = 0_s;
  bool m_running = false;
};

}  // namespace frc
