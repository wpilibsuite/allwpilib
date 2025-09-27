// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/ParallelCommandGroup.h"

#include <utility>
#include <vector>

using namespace frc2;

ParallelCommandGroup::ParallelCommandGroup(
    std::vector<std::unique_ptr<Command>>&& commands) {
  AddCommands(std::move(commands));
}

void ParallelCommandGroup::Initialize() {
  for (auto& commandRunning : m_commands) {
    commandRunning.first->Initialize();
    commandRunning.second = true;
  }
  isRunning = true;
}

void ParallelCommandGroup::Execute() {
  for (auto& commandRunning : m_commands) {
    if (!commandRunning.second) {
      continue;
    }
    commandRunning.first->Execute();
    if (commandRunning.first->IsFinished()) {
      commandRunning.first->End(false);
      commandRunning.second = false;
    }
  }
}

void ParallelCommandGroup::End(bool interrupted) {
  if (interrupted) {
    for (auto& commandRunning : m_commands) {
      if (commandRunning.second) {
        commandRunning.first->End(true);
      }
    }
  }
  isRunning = false;
}

bool ParallelCommandGroup::IsFinished() {
  for (auto& command : m_commands) {
    if (command.second) {
      return false;
    }
  }
  return true;
}

bool ParallelCommandGroup::RunsWhenDisabled() const {
  return m_runWhenDisabled;
}

Command::InterruptionBehavior ParallelCommandGroup::GetInterruptionBehavior()
    const {
  return m_interruptBehavior;
}

void ParallelCommandGroup::AddCommands(
    std::vector<std::unique_ptr<Command>>&& commands) {
  CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(commands);

  if (isRunning) {
    throw FRC_MakeError(frc::err::CommandIllegalUse,
                        "Commands cannot be added to a CommandGroup "
                        "while the group is running");
  }

  for (auto&& command : commands) {
    if (RequirementsDisjoint(this, command.get())) {
      command->SetComposed(true);
      AddRequirements(command->GetRequirements());
      m_runWhenDisabled &= command->RunsWhenDisabled();
      if (command->GetInterruptionBehavior() ==
          Command::InterruptionBehavior::kCancelSelf) {
        m_interruptBehavior = Command::InterruptionBehavior::kCancelSelf;
      }
      m_commands.emplace_back(std::move(command), false);
    } else {
      throw FRC_MakeError(frc::err::CommandIllegalUse,
                          "Multiple commands in a parallel group cannot "
                          "require the same subsystems");
    }
  }
}
