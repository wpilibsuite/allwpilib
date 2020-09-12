/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Timer.h"

#include <units/time.h>

namespace frc {

void Wait(double seconds) { frc2::Wait(units::second_t(seconds)); }

double GetTime() { return frc2::GetTime().to<double>(); }

}  // namespace frc

using namespace frc;

Timer::Timer() { Reset(); }

double Timer::Get() const { return m_timer.Get().to<double>(); }

void Timer::Reset() { m_timer.Reset(); }

void Timer::Start() { m_timer.Start(); }

void Timer::Stop() { m_timer.Stop(); }

bool Timer::HasPeriodPassed(double period) {
  return m_timer.HasPeriodPassed(units::second_t(period));
}

double Timer::GetFPGATimestamp() {
  return frc2::Timer::GetFPGATimestamp().to<double>();
}

double Timer::GetMatchTime() {
  return frc2::Timer::GetMatchTime().to<double>();
}
