// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Timer.hpp"

#include <stdint.h>

#include <chrono>
#include <thread>

#include "wpi/driverstation/MatchState.hpp"
#include "wpi/system/RobotController.hpp"

namespace wpi {

void Wait(wpi::units::second_t seconds) {
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds.value()));
}

wpi::units::second_t GetSystemTime() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::system_clock;

  return wpi::units::second_t{
      duration_cast<duration<double>>(system_clock::now().time_since_epoch())
          .count()};
}

}  // namespace wpi

using namespace wpi;

namespace {

std::chrono::microseconds GetTimestampMicroseconds() {
  return std::chrono::microseconds{
      static_cast<int64_t>(wpi::RobotController::GetTime())};
}

}  // namespace

Timer::Timer() {
  Reset();
}

wpi::units::second_t Timer::Get() const {
  return wpi::units::microsecond_t{GetMicroseconds()};
}

double Timer::GetMicroseconds() const {
  if (m_running) {
    return static_cast<double>(
               (GetTimestampMicroseconds() - m_startTime).count()) -
           m_startTimeRemainderUs + m_accumulatedTimeUs;
  } else {
    return m_accumulatedTimeUs;
  }
}

void Timer::Reset() {
  m_accumulatedTimeUs = 0.0;
  m_startTime = GetTimestampMicroseconds();
  m_startTimeRemainderUs = 0.0;
}

void Timer::Start() {
  if (!m_running) {
    m_startTime = GetTimestampMicroseconds();
    m_startTimeRemainderUs = 0.0;
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
    m_accumulatedTimeUs = GetMicroseconds();
    m_running = false;
  }
}

bool Timer::HasElapsed(wpi::units::second_t period) const {
  return GetMicroseconds() >= wpi::units::microsecond_t{period}.value();
}

bool Timer::AdvanceIfElapsed(wpi::units::second_t period) {
  double periodUs = wpi::units::microsecond_t{period}.value();

  if (GetMicroseconds() >= periodUs) {
    // Advance the start time by the period.
    double advanceUs = m_startTimeRemainderUs + periodUs;
    auto wholeUs = static_cast<int64_t>(advanceUs);
    m_startTime += std::chrono::microseconds{wholeUs};
    m_startTimeRemainderUs = advanceUs - wholeUs;
    // Don't set it to the current time... we want to avoid drift.
    return true;
  } else {
    return false;
  }
}

bool Timer::IsRunning() const {
  return m_running;
}

Timer Timer::CreateStarted() {
  Timer timer;
  timer.Start();
  return timer;
}

wpi::units::second_t Timer::GetTimestamp() {
  return GetTimestampMicroseconds();
}

wpi::units::second_t Timer::GetMonotonicTimestamp() {
  return std::chrono::microseconds{
      static_cast<int64_t>(wpi::RobotController::GetMonotonicTime())};
}

wpi::units::second_t Timer::GetMatchTime() {
  return wpi::MatchState::GetMatchTime();
}
