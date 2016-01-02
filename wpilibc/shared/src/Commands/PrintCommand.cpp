/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/PrintCommand.h"
#include "stdio.h"
#include <sstream>

PrintCommand::PrintCommand(const std::string &message)
    : Command(((std::stringstream &)(std::stringstream("Print \"") << message
                                                                   << "\""))
                  .str()
                  .c_str()) {
  m_message = message;
}

void PrintCommand::Initialize() { printf("%s", m_message.c_str()); }

void PrintCommand::Execute() {}

bool PrintCommand::IsFinished() { return true; }

void PrintCommand::End() {}

void PrintCommand::Interrupted() {}
