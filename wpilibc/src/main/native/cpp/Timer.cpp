/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Timer.h"

#include <chrono>
#include <thread>

#include <hal/HAL.h>

#include "frc/DriverStation.h"
#include "frc/RobotController.h"

namespace frc {

void Wait(double seconds) {
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

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

Timer::Timer() { Reset(); }

Timer::Timer(Timer&& rhs)
    : m_startTime(std::move(rhs.m_startTime)),
      m_accumulatedTime(std::move(rhs.m_accumulatedTime)),
      m_running(std::move(rhs.m_running)) {}

Timer& Timer::operator=(Timer&& rhs) {
  std::scoped_lock lock(m_mutex, rhs.m_mutex);

  m_startTime = std::move(rhs.m_startTime);
  m_accumulatedTime = std::move(rhs.m_accumulatedTime);
  m_running = std::move(rhs.m_running);

  return *this;
}

double Timer::Get() const {
  double result;
  double currentTime = GetFPGATimestamp();

  std::scoped_lock lock(m_mutex);
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

void Timer::Reset() {
  std::scoped_lock lock(m_mutex);
  m_accumulatedTime = 0;
  m_startTime = GetFPGATimestamp();
}

void Timer::Start() {
  std::scoped_lock lock(m_mutex);
  if (!m_running) {
    m_startTime = GetFPGATimestamp();
    m_running = true;
  }
}

void Timer::Stop() {
  double temp = Get();

  std::scoped_lock lock(m_mutex);
  if (m_running) {
    m_accumulatedTime = temp;
    m_running = false;
  }
}

bool Timer::HasPeriodPassed(double period) {
  return HasPeriodPassed(units::second_t(period));
}

bool Timer::HasPeriodPassed(units::second_t period) {
  if (Get() > period.to<double>()) {
    std::scoped_lock lock(m_mutex);
    // Advance the start time by the period.
    m_startTime += period.to<double>();
    // Don't set it to the current time... we want to avoid drift.
    return true;
  }
  return false;
}

double Timer::GetFPGATimestamp() {
  // FPGA returns the timestamp in microseconds
  return RobotController::GetFPGATime() * 1.0e-6;
}

double Timer::GetMatchTime() {
  return DriverStation::GetInstance().GetMatchTime();
}
