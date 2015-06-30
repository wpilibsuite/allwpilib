/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitCommand.h"
#include <sstream>

WaitCommand::WaitCommand(double timeout)
    : Command(
          ((std::stringstream &)(std::stringstream("Wait(") << timeout << ")"))
              .str()
              .c_str(),
          timeout) {}

WaitCommand::WaitCommand(const std::string &name, double timeout)
    : Command(name, timeout) {}

void WaitCommand::Initialize() {}

void WaitCommand::Execute() {}

bool WaitCommand::IsFinished() { return IsTimedOut(); }

void WaitCommand::End() {}

void WaitCommand::Interrupted() {}
