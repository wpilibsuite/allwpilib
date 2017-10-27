/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/ConditionalCommand.h"

#include "Commands/Scheduler.h"

using namespace frc;

/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
 *
 * @param onTrue  The Command to execute if Condition() returns true
 * @param onFalse The Command to execute if Condition() returns false
 */
ConditionalCommand::ConditionalCommand(Command* onTrue, Command* onFalse) {
  m_onTrue = ProcessCommand(onTrue);
  m_onFalse = ProcessCommand(onFalse);
}

/**
 * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
 *
 * @param name    The name for this command group
 * @param onTrue  The Command to execute if Condition() returns true
 * @param onFalse The Command to execute if Condition() returns false
 */
ConditionalCommand::ConditionalCommand(const llvm::Twine& name, Command* onTrue,
                                       Command* onFalse)
    : CommandGroup(name) {
  m_onTrue = ProcessCommand(onTrue);
  m_onFalse = ProcessCommand(onFalse);
}

void ConditionalCommand::_Initialize() {
  CommandGroup::_Initialize();

  if (Condition()) {
    SetCommands(m_onTrue);
  } else {
    SetCommands(m_onFalse);
  }
}

std::vector<CommandGroupEntry> ConditionalCommand::ProcessCommand(
    Command* cmd) {
  std::vector<CommandGroupEntry> list;
  if (cmd == nullptr) {
    return list;
  }

  if (!AssertUnlocked("Cannot add conditions to conditional command"))
    return list;

  for (auto requirement : cmd->GetRequirements()) Requires(requirement);

  cmd->SetParent(this);
  list.push_back(
      CommandGroupEntry(cmd, CommandGroupEntry::kSequence_InSequence));

  return list;
}
