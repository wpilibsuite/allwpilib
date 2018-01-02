/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <support/deprecated.h>
#include <support/mutex.h>

#include "Base.h"

namespace frc {

typedef void (*TimerInterruptHandler)(void* param);

void Wait(double seconds);
WPI_DEPRECATED("Use Timer::GetFPGATimestamp() instead.")
double GetClock();
double GetTime();

/**
 * Timer objects measure accumulated time in seconds.
 *
 * The timer object functions like a stopwatch. It can be started, stopped, and
 * cleared. When the timer is running its value counts up in seconds. When
 * stopped, the timer holds the current value. The implementation simply records
 * the time when started and subtracts the current time whenever the value is
 * requested.
 */
class Timer {
 public:
  Timer();
  virtual ~Timer() = default;

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  double Get() const;
  void Reset();
  void Start();
  void Stop();
  bool HasPeriodPassed(double period);

  static double GetFPGATimestamp();
  static double GetMatchTime();

  // The time, in seconds, at which the 32-bit FPGA timestamp rolls over to 0
  static const double kRolloverTime;

 private:
  double m_startTime = 0.0;
  double m_accumulatedTime = 0.0;
  bool m_running = false;
  mutable wpi::mutex m_mutex;
};

}  // namespace frc
