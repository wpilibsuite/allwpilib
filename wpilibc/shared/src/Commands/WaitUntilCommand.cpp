/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitUntilCommand.h"
#include "Timer.h"

/**
 * A {@link WaitCommand} will wait until a certain match time before finishing.
 * This will wait until the game clock reaches some value, then continue to the
 * next command.
 * @see CommandGroup
 */
WaitUntilCommand::WaitUntilCommand(double time)
    : Command("WaitUntilCommand", time) {
  m_time = time;
}

WaitUntilCommand::WaitUntilCommand(const std::string &name, double time)
    : Command(name, time) {
  m_time = time;
}

void WaitUntilCommand::Initialize() {}

void WaitUntilCommand::Execute() {}

/**
 * Check if we've reached the actual finish time.
 */
bool WaitUntilCommand::IsFinished() { return Timer::GetMatchTime() >= m_time; }

void WaitUntilCommand::End() {}

void WaitUntilCommand::Interrupted() {}
