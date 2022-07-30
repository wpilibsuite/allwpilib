// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command/EndlessCommand.h"

using namespace frc;

EndlessCommand::EndlessCommand(std::unique_ptr<Command>&& command) {
  if (!CommandGroupBase::RequireUngrouped(*command)) {
    return;
  }
  m_command = std::move(command);
  m_command->SetGrouped(true);
  AddRequirements(m_command->GetRequirements());
}

void EndlessCommand::Initialize() {
  m_command->Initialize();
}

void EndlessCommand::Execute() {
  m_command->Execute();
}

void EndlessCommand::End(bool interrupted) {
  m_command->End(interrupted);
}
