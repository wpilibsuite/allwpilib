// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/PerpetualCommand.h"

using namespace frc2;

PerpetualCommand::PerpetualCommand(std::unique_ptr<Command>&& command) {
  if (!CommandGroupBase::RequireUngrouped(*command)) {
    return;
  }
  m_command = std::move(command);
  m_command->SetGrouped(true);
  AddRequirements(m_command->GetRequirements());
}

void PerpetualCommand::Initialize() {
  m_command->Initialize();
}

void PerpetualCommand::Execute() {
  m_command->Execute();
}

void PerpetualCommand::End(bool interrupted) {
  m_command->End(interrupted);
}
