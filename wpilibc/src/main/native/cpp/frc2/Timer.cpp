/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/Timer.h"

#include <chrono>
#include <thread>

#include <hal/FRCUsageReporting.h>

#include "frc/DriverStation.h"
#include "frc/RobotController.h"

namespace frc2 {

void Wait(units::second_t seconds) {
  std::this_thread::sleep_for(
      std::chrono::duration<double>(seconds.to<double>()));
}

units::second_t GetTime() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::system_clock;

  return units::second_t(
      duration_cast<duration<double>>(system_clock::now().time_since_epoch())
          .count());
}

}  // namespace frc2

using namespace frc2;

// for compatibility with msvc12--see C2864
const units::second_t Timer::kRolloverTime = units::second_t((1ll << 32) / 1e6);

Timer::Timer() { Reset(); }

Timer::Timer(const Timer& rhs)
    : m_startTime(rhs.m_startTime),
      m_accumulatedTime(rhs.m_accumulatedTime),
      m_running(rhs.m_running) {}

Timer& Timer::operator=(const Timer& rhs) {
  std::scoped_lock lock(m_mutex, rhs.m_mutex);

  m_startTime = rhs.m_startTime;
  m_accumulatedTime = rhs.m_accumulatedTime;
  m_running = rhs.m_running;

  return *this;
}

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

units::second_t Timer::Get() const {
  units::second_t result;
  units::second_t currentTime = GetFPGATimestamp();

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
  m_accumulatedTime = 0_s;
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
  units::second_t temp = Get();

  std::scoped_lock lock(m_mutex);
  if (m_running) {
    m_accumulatedTime = temp;
    m_running = false;
  }
}

bool Timer::HasPeriodPassed(units::second_t period) {
  if (Get() > period) {
    std::scoped_lock lock(m_mutex);
    // Advance the start time by the period.
    m_startTime += period;
    // Don't set it to the current time... we want to avoid drift.
    return true;
  }
  return false;
}

units::second_t Timer::GetFPGATimestamp() {
  // FPGA returns the timestamp in microseconds
  return units::second_t(frc::RobotController::GetFPGATime() * 1.0e-6);
}

units::second_t Timer::GetMatchTime() {
  return units::second_t(frc::DriverStation::GetInstance().GetMatchTime());
}
