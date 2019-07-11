/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/StartCommand.h"

using namespace frc;

StartCommand::StartCommand(Command* commandToStart)
    : InstantCommand("StartCommand") {
  m_commandToFork = commandToStart;
}

void StartCommand::Initialize() { m_commandToFork->Start(); }
