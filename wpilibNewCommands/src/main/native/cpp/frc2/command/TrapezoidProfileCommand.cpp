/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/TrapezoidProfileCommand.h"

using namespace frc2;

TrapezoidProfileCommand::TrapezoidProfileCommand(
    frc::TrapezoidProfile profile,
    std::function<void(frc::TrapezoidProfile::State)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_profile(profile), m_output(output) {
  AddRequirements(requirements);
}

void TrapezoidProfileCommand::Initialize() {
  m_timer.Reset();
  m_timer.Start();
}

void TrapezoidProfileCommand::Execute() {
  m_output(m_profile.Calculate(m_timer.Get()));
}

void TrapezoidProfileCommand::End(bool interrupted) { m_timer.Stop(); }

bool TrapezoidProfileCommand::IsFinished() {
  return m_timer.HasPeriodPassed(m_profile.TotalTime());
}
