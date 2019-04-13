/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/PrintCommand.h"

#include <wpi/raw_ostream.h>
#pragma warning(disable: 4244 4267 4146)

using namespace frc;

PrintCommand::PrintCommand(const wpi::Twine& message)
    : InstantCommand("Print \"" + message + wpi::Twine('"')) {
  m_message = message.str();
}

void PrintCommand::Initialize() { wpi::outs() << m_message << '\n'; }
