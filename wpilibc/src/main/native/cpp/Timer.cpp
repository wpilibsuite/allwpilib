// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Timer.h"

#include <units/time.h>

namespace frc {

void Wait(double seconds) {
  frc2::Wait(units::second_t(seconds));
}

double GetTime() {
  return frc2::GetTime().to<double>();
}

}  // namespace frc

using namespace frc;

Timer::Timer() {
  Reset();
}

double Timer::Get() const {
  return m_timer.Get().to<double>();
}

void Timer::Reset() {
  m_timer.Reset();
}

void Timer::Start() {
  m_timer.Start();
}

void Timer::Stop() {
  m_timer.Stop();
}

bool Timer::HasPeriodPassed(double period) {
  return m_timer.HasPeriodPassed(units::second_t(period));
}

double Timer::GetFPGATimestamp() {
  return frc2::Timer::GetFPGATimestamp().to<double>();
}

double Timer::GetMatchTime() {
  return frc2::Timer::GetMatchTime().to<double>();
}
