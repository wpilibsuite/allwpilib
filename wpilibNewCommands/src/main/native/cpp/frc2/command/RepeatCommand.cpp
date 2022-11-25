// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/RepeatCommand.h"

using namespace frc2;

RepeatCommand::RepeatCommand(std::unique_ptr<Command>&& command) {
  if (!CommandGroupBase::RequireUngrouped(*command)) {
    return;
  }
  m_command = std::move(command);
  m_command->SetGrouped(true);
  AddRequirements(m_command->GetRequirements());
}

void RepeatCommand::Initialize() {
  m_ended = false;
  m_command->Initialize();
}

void RepeatCommand::Execute() {
  if (m_ended) {
    m_ended = false;
    m_command->Initialize();
  }
  m_command->Execute();
  if (m_command->IsFinished()) {
    // restart command
    m_command->End(false);
    m_ended = true;
  }
}

bool RepeatCommand::IsFinished() {
  return false;
}

void RepeatCommand::End(bool interrupted) {
  m_command->End(interrupted);
}

bool RepeatCommand::RunsWhenDisabled() const {
  return m_command->RunsWhenDisabled();
}

Command::InterruptionBehavior RepeatCommand::GetInterruptionBehavior() const {
  return m_command->GetInterruptionBehavior();
}
