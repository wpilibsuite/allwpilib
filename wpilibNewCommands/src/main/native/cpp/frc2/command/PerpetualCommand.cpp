/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/PerpetualCommand.h"

using namespace frc2;

PerpetualCommand::PerpetualCommand(std::unique_ptr<Command>&& command) {
  if (!CommandGroupBase::RequireUngrouped(command)) {
    return;
  }
  m_command = std::move(command);
  m_command->SetGrouped(true);
  AddRequirements(m_command->GetRequirements());
}

void PerpetualCommand::Initialize() { m_command->Initialize(); }

void PerpetualCommand::Execute() { m_command->Execute(); }

void PerpetualCommand::End(bool interrupted) { m_command->End(interrupted); }
