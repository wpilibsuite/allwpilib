/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/ParallelDeadlineGroup.h"

using namespace frc2;

ParallelDeadlineGroup::ParallelDeadlineGroup(
    std::unique_ptr<Command>&& deadline,
    std::vector<std::unique_ptr<Command>>&& commands) {
  SetDeadline(std::move(deadline));
  AddCommands(std::move(commands));
}

void ParallelDeadlineGroup::Initialize() {
  for (auto& commandRunning : m_commands) {
    commandRunning.first->Initialize();
    commandRunning.second = true;
  }
  isRunning = true;
}

void ParallelDeadlineGroup::Execute() {
  for (auto& commandRunning : m_commands) {
    if (!commandRunning.second) continue;
    commandRunning.first->Execute();
    if (commandRunning.first->IsFinished()) {
      commandRunning.first->End(false);
      commandRunning.second = false;
    }
  }
}

void ParallelDeadlineGroup::End(bool interrupted) {
  for (auto& commandRunning : m_commands) {
    if (commandRunning.second) {
      commandRunning.first->End(true);
    }
  }
  isRunning = false;
}

bool ParallelDeadlineGroup::IsFinished() { return m_deadline->IsFinished(); }

bool ParallelDeadlineGroup::RunsWhenDisabled() const {
  return m_runWhenDisabled;
}

void ParallelDeadlineGroup::AddCommands(
    std::vector<std::unique_ptr<Command>>&& commands) {
  if (!RequireUngrouped(commands)) {
    return;
  }

  if (isRunning) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "Commands cannot be added to a CommandGroup "
                               "while the group is running");
  }

  for (auto&& command : commands) {
    if (RequirementsDisjoint(this, command.get())) {
      command->SetGrouped(true);
      AddRequirements(command->GetRequirements());
      m_runWhenDisabled &= command->RunsWhenDisabled();
      m_commands.emplace_back(std::move(command), false);
    } else {
      wpi_setWPIErrorWithContext(CommandIllegalUse,
                                 "Multiple commands in a parallel group cannot "
                                 "require the same subsystems");
      return;
    }
  }
}

void ParallelDeadlineGroup::SetDeadline(std::unique_ptr<Command>&& deadline) {
  m_deadline = deadline.get();
  m_deadline->SetGrouped(true);
  m_commands.emplace_back(std::move(deadline), false);
  AddRequirements(m_deadline->GetRequirements());
  m_runWhenDisabled &= m_deadline->RunsWhenDisabled();
}
