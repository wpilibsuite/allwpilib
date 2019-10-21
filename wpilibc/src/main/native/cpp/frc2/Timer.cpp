/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Timer.h"
#include "frc2/Timer.h"

namespace frc2 {
void Wait(units::second_t seconds) { frc::Wait(seconds.to<double>()); }

units::second_t GetTime() { return units::second_t(frc::GetTime()); }

units::second_t Timer::Get() const { return units::second_t(m_timer.Get()); }

void Timer::Reset() { m_timer.Reset(); }

void Timer::Start() { m_timer.Start(); }

void Timer::Stop() { m_timer.Stop(); }

bool Timer::HasPeriodPassed(units::second_t period) {
  return m_timer.HasPeriodPassed(period);
}

units::second_t Timer::GetFPGATimestamp() {
  return units::second_t(frc::Timer::GetFPGATimestamp());
}

units::second_t Timer::GetMatchTime() {
  return units::second_t(frc::Timer::GetMatchTime());
}

const units::second_t Timer::kRolloverTime =
    units::second_t(frc::Timer::kRolloverTime);

}  // namespace frc2
