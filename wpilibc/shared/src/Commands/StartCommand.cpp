/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/StartCommand.h"

StartCommand::StartCommand(Command *commandToStart) : Command("StartCommand") {
  m_commandToFork = commandToStart;
}

void StartCommand::Initialize() { m_commandToFork->Start(); }

void StartCommand::Execute() {}

void StartCommand::End() {}

void StartCommand::Interrupted() {}

bool StartCommand::IsFinished() { return true; }
