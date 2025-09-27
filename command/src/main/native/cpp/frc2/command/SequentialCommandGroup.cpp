// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SequentialCommandGroup.h"

#include <utility>
#include <vector>

#include <wpi/sendable/SendableBuilder.h>

using namespace frc2;

SequentialCommandGroup::SequentialCommandGroup(
    std::vector<std::unique_ptr<Command>>&& commands) {
  AddCommands(std::move(commands));
}

void SequentialCommandGroup::Initialize() {
  m_currentCommandIndex = 0;

  if (!m_commands.empty()) {
    m_commands[0]->Initialize();
  }
}

void SequentialCommandGroup::Execute() {
  if (m_commands.empty()) {
    return;
  }

  auto& currentCommand = m_commands[m_currentCommandIndex];

  currentCommand->Execute();
  if (currentCommand->IsFinished()) {
    currentCommand->End(false);
    m_currentCommandIndex++;
    if (m_currentCommandIndex < m_commands.size()) {
      m_commands[m_currentCommandIndex]->Initialize();
    }
  }
}

void SequentialCommandGroup::End(bool interrupted) {
  if (interrupted && !m_commands.empty() &&
      m_currentCommandIndex != invalid_index &&
      m_currentCommandIndex < m_commands.size()) {
    m_commands[m_currentCommandIndex]->End(interrupted);
  }
  m_currentCommandIndex = invalid_index;
}

bool SequentialCommandGroup::IsFinished() {
  return m_currentCommandIndex == m_commands.size();
}

bool SequentialCommandGroup::RunsWhenDisabled() const {
  return m_runWhenDisabled;
}

Command::InterruptionBehavior SequentialCommandGroup::GetInterruptionBehavior()
    const {
  return m_interruptBehavior;
}

void SequentialCommandGroup::AddCommands(
    std::vector<std::unique_ptr<Command>>&& commands) {
  CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(commands);

  if (m_currentCommandIndex != invalid_index) {
    throw FRC_MakeError(frc::err::CommandIllegalUse,
                        "Commands cannot be added to a CommandGroup "
                        "while the group is running");
  }

  for (auto&& command : commands) {
    command->SetComposed(true);
    AddRequirements(command->GetRequirements());
    m_runWhenDisabled &= command->RunsWhenDisabled();
    if (command->GetInterruptionBehavior() ==
        Command::InterruptionBehavior::kCancelSelf) {
      m_interruptBehavior = Command::InterruptionBehavior::kCancelSelf;
    }
    m_commands.emplace_back(std::move(command));
  }
}

void SequentialCommandGroup::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddIntegerProperty(
      "index", [this] { return m_currentCommandIndex; }, nullptr);
}
