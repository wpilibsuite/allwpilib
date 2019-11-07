/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/PrintCommand.h"

#include <wpi/raw_ostream.h>

using namespace frc2;

PrintCommand::PrintCommand(const wpi::Twine& message)
    : CommandHelper{[str = message.str()] { wpi::outs() << str << "\n"; }, {}} {
}

bool PrintCommand::RunsWhenDisabled() const { return true; }
