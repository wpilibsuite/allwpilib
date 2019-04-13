/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/WaitUntilCommand.h"

#include "frc/Timer.h"

using namespace frc;

WaitUntilCommand::WaitUntilCommand(double time)
    : Command("WaitUntilCommand", time) {
  m_time = time;
}

WaitUntilCommand::WaitUntilCommand(const wpi::Twine& name, double time)
    : Command(name, time) {
  m_time = time;
}

bool WaitUntilCommand::IsFinished() { return Timer::GetMatchTime() >= m_time; }
