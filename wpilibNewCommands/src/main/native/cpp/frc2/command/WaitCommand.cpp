/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/WaitCommand.h"

using namespace frc2;

WaitCommand::WaitCommand(units::second_t duration) : m_duration{duration} {
  auto durationStr = std::to_string(duration.to<double>());
  SetName(wpi::Twine(GetName()) + ": " + wpi::Twine(durationStr) + " seconds");
}

void WaitCommand::Initialize() {
  m_timer.Reset();
  m_timer.Start();
}

void WaitCommand::End(bool interrupted) { m_timer.Stop(); }

bool WaitCommand::IsFinished() { return m_timer.HasElapsed(m_duration); }

bool WaitCommand::RunsWhenDisabled() const { return true; }
