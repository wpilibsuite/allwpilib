/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/CommandGroup.h"

#include "frc/WPIErrors.h"

using namespace frc;

CommandGroup::CommandGroup(const wpi::Twine& name) : Command(name) {}

void CommandGroup::AddSequential(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_InSequence);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto&& requirement : command->GetRequirements()) Requires(requirement);
}

void CommandGroup::AddSequential(Command* command, double timeout) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;
  if (timeout < 0.0) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");
    return;
  }

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_InSequence,
                          timeout);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto&& requirement : command->GetRequirements()) Requires(requirement);
}

void CommandGroup::AddParallel(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_BranchChild);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto&& requirement : command->GetRequirements()) Requires(requirement);
}

void CommandGroup::AddParallel(Command* command, double timeout) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;
  if (timeout < 0.0) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");
    return;
  }

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_BranchChild,
                          timeout);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto&& requirement : command->GetRequirements()) Requires(requirement);
}

bool CommandGroup::IsInterruptible() const {
  if (!Command::IsInterruptible()) return false;

  if (m_currentCommandIndex != -1 &&
      static_cast<size_t>(m_currentCommandIndex) < m_commands.size()) {
    Command* cmd = m_commands[m_currentCommandIndex].m_command;
    if (!cmd->IsInterruptible()) return false;
  }

  for (const auto& child : m_children) {
    if (!child->m_command->IsInterruptible()) return false;
  }

  return true;
}

int CommandGroup::GetSize() const { return m_children.size(); }

void CommandGroup::Initialize() {}

void CommandGroup::Execute() {}

bool CommandGroup::IsFinished() {
  return static_cast<size_t>(m_currentCommandIndex) >= m_commands.size() &&
         m_children.empty();
}

void CommandGroup::End() {}

void CommandGroup::Interrupted() {}

void CommandGroup::_Initialize() { m_currentCommandIndex = -1; }

void CommandGroup::_Execute() {
  CommandGroupEntry* entry;
  Command* cmd = nullptr;
  bool firstRun = false;

  if (m_currentCommandIndex == -1) {
    firstRun = true;
    m_currentCommandIndex = 0;
  }

  // While there are still commands in this group to run
  while (static_cast<size_t>(m_currentCommandIndex) < m_commands.size()) {
    // If a command is prepared to run
    if (cmd != nullptr) {
      // If command timed out, cancel it so it's removed from the Scheduler
      if (entry->IsTimedOut()) cmd->_Cancel();

      // If command finished or was cancelled, remove it from Scheduler
      if (cmd->Run()) {
        break;
      } else {
        cmd->Removed();

        // Advance to next command in group
        m_currentCommandIndex++;
        firstRun = true;
        cmd = nullptr;
        continue;
      }
    }

    entry = &m_commands[m_currentCommandIndex];
    cmd = nullptr;

    switch (entry->m_state) {
      case CommandGroupEntry::kSequence_InSequence:
        cmd = entry->m_command;
        if (firstRun) {
          cmd->StartRunning();
          CancelConflicts(cmd);
          firstRun = false;
        }
        break;

      case CommandGroupEntry::kSequence_BranchPeer:
        // Start executing a parallel command and advance to next entry in group
        m_currentCommandIndex++;
        entry->m_command->Start();
        break;

      case CommandGroupEntry::kSequence_BranchChild:
        m_currentCommandIndex++;

        /* Causes scheduler to skip children of current command which require
         * the same subsystems as it
         */
        CancelConflicts(entry->m_command);
        entry->m_command->StartRunning();

        // Add current command entry to list of children of this group
        m_children.push_back(entry);
        break;
    }
  }

  // Run Children
  for (auto& entry : m_children) {
    auto child = entry->m_command;
    if (entry->IsTimedOut()) {
      child->_Cancel();
    }

    // If child finished or was cancelled, set it to nullptr. nullptr entries
    // are removed later.
    if (!child->Run()) {
      child->Removed();
      entry = nullptr;
    }
  }

  m_children.erase(std::remove(m_children.begin(), m_children.end(), nullptr),
                   m_children.end());
}

void CommandGroup::_End() {
  // Theoretically, we don't have to check this, but we do if teams override the
  // IsFinished method
  if (m_currentCommandIndex != -1 &&
      static_cast<size_t>(m_currentCommandIndex) < m_commands.size()) {
    Command* cmd = m_commands[m_currentCommandIndex].m_command;
    cmd->_Cancel();
    cmd->Removed();
  }

  for (auto& child : m_children) {
    Command* cmd = child->m_command;
    cmd->_Cancel();
    cmd->Removed();
  }
  m_children.clear();
}

void CommandGroup::_Interrupted() { _End(); }

void CommandGroup::CancelConflicts(Command* command) {
  for (auto childIter = m_children.begin(); childIter != m_children.end();) {
    Command* child = (*childIter)->m_command;
    bool erased = false;

    for (auto&& requirement : command->GetRequirements()) {
      if (child->DoesRequire(requirement)) {
        child->_Cancel();
        child->Removed();
        childIter = m_children.erase(childIter);
        erased = true;
        break;
      }
    }
    if (!erased) childIter++;
  }
}
