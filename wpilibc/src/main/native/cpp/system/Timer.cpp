// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Timer.hpp"

#include <chrono>
#include <thread>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/system/RobotController.hpp"

namespace wpi {

void Wait(wpi::units::second_t seconds) {
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds.value()));
}

wpi::units::second_t GetTime() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::system_clock;

  return wpi::units::second_t{
      duration_cast<duration<double>>(system_clock::now().time_since_epoch())
          .count()};
}

}  // namespace wpi

using namespace wpi;

Timer::Timer() {
  Reset();
}

wpi::units::second_t Timer::Get() const {
  if (m_running) {
    return (GetTimestamp() - m_startTime) + m_accumulatedTime;
  } else {
    return m_accumulatedTime;
  }
}

void Timer::Reset() {
  m_accumulatedTime = 0_s;
  m_startTime = GetTimestamp();
}

void Timer::Start() {
  if (!m_running) {
    m_startTime = GetTimestamp();
    m_running = true;
  }
}

void Timer::Restart() {
  if (m_running) {
    Stop();
  }
  Reset();
  Start();
}

void Timer::Stop() {
  if (m_running) {
    m_accumulatedTime = Get();
    m_running = false;
  }
}

bool Timer::HasElapsed(wpi::units::second_t period) const {
  return Get() >= period;
}

bool Timer::AdvanceIfElapsed(wpi::units::second_t period) {
  if (Get() >= period) {
    // Advance the start time by the period.
    m_startTime += period;
    // Don't set it to the current time... we want to avoid drift.
    return true;
  } else {
    return false;
  }
}

bool Timer::IsRunning() const {
  return m_running;
}

wpi::units::second_t Timer::GetTimestamp() {
  return wpi::units::second_t{wpi::RobotController::GetTime() * 1.0e-6};
}

wpi::units::second_t Timer::GetFPGATimestamp() {
  // FPGA returns the timestamp in microseconds
  return wpi::units::second_t{wpi::RobotController::GetFPGATime() * 1.0e-6};
}

wpi::units::second_t Timer::GetMatchTime() {
  return wpi::DriverStation::GetMatchTime();
}
