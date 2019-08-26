/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/WaitCommand.h"

using namespace frc2;

WaitCommand::WaitCommand(double seconds) : m_duration{seconds} {
  auto secondsStr = std::to_string(seconds);
  SetName(wpi::Twine(m_name) + ": " + wpi::Twine(secondsStr) + " seconds");
  m_timer = std::make_unique<frc::Timer>();
}

void WaitCommand::Initialize() {
  m_timer->Reset();
  m_timer->Start();
}

void WaitCommand::End(bool interrupted) { m_timer->Stop(); }

bool WaitCommand::IsFinished() { return m_timer->HasPeriodPassed(m_duration); }

bool WaitCommand::RunsWhenDisabled() const { return true; }
