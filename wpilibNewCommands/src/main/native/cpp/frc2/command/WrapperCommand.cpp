// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/WrapperCommand.h"

#include <utility>

#include "frc2/command/Command.h"

using namespace frc2;

WrapperCommand::WrapperCommand(std::unique_ptr<Command>&& command) {
  CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(command.get());
  m_command = std::move(command);
  m_command->SetComposed(true);
  // copy the wrapped command's name
  SetName(m_command->GetName());
}

void WrapperCommand::Initialize() {
  m_command->Initialize();
}

void WrapperCommand::Execute() {
  m_command->Execute();
}

bool WrapperCommand::IsFinished() {
  return m_command->IsFinished();
}

void WrapperCommand::End(bool interrupted) {
  m_command->End(interrupted);
}

bool WrapperCommand::RunsWhenDisabled() const {
  return m_command->RunsWhenDisabled();
}

Command::InterruptionBehavior WrapperCommand::GetInterruptionBehavior() const {
  return m_command->GetInterruptionBehavior();
}

wpi::SmallSet<Subsystem*, 4> WrapperCommand::GetRequirements() const {
  return m_command->GetRequirements();
}
