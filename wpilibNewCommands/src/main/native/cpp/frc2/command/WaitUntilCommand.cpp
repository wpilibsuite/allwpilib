/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;

WaitUntilCommand::WaitUntilCommand(std::function<bool()> condition)
    : m_condition{std::move(condition)} {}

WaitUntilCommand::WaitUntilCommand(double time)
    : m_condition{[=] { return frc::Timer::GetMatchTime() - time > 0; }} {}

bool WaitUntilCommand::IsFinished() { return m_condition(); }

bool WaitUntilCommand::RunsWhenDisabled() const { return true; }
