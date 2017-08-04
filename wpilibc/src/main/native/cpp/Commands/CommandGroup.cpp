/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/CommandGroup.h"

#include "WPIErrors.h"

using namespace frc;

/**
 * Creates a new {@link CommandGroup CommandGroup} with the given name.
 * @param name the name for this command group
 */
CommandGroup::CommandGroup(const std::string& name) : Command(name) {}

/**
 * Adds a new {@link Command Command} to the group.  The {@link Command Command}
 * will be started after all the previously added {@link Command Commands}.
 *
 * <p>Note that any requirements the given {@link Command Command} has will be
 * added to the group.  For this reason, a {@link Command Command's}
 * requirements can not be changed after being added to a group.</p>
 *
 * <p>It is recommended that this method be called in the constructor.</p>
 *
 * @param command The {@link Command Command} to be added
 */
void CommandGroup::AddSequential(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;

  command->SetParent(this);

  m_commands.push_back(
      CommandGroupEntry(command, CommandGroupEntry::kSequence_InSequence));
  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  Command::SubsystemSet requirements = command->GetRequirements();
  for (auto iter = requirements.begin(); iter != requirements.end(); iter++)
    Requires(*iter);
}

/**
 * Adds a new {@link Command Command} to the group with a given timeout.
 * The {@link Command Command} will be started after all the previously added
 * commands.
 *
 * <p>Once the {@link Command Command} is started, it will be run until it
 * finishes or the time expires, whichever is sooner.  Note that the given
 * {@link Command Command} will have no knowledge that it is on a timer.</p>
 *
 * <p>Note that any requirements the given {@link Command Command} has will be
 * added to the group.  For this reason, a {@link Command Command's}
 * requirements can not be changed after being added to a group.</p>
 *
 * <p>It is recommended that this method be called in the constructor.</p>
 *
 * @param command The {@link Command Command} to be added
 * @param timeout The timeout (in seconds)
 */
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

  command->SetParent(this);

  m_commands.push_back(CommandGroupEntry(
      command, CommandGroupEntry::kSequence_InSequence, timeout));
  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  Command::SubsystemSet requirements = command->GetRequirements();
  for (auto iter = requirements.begin(); iter != requirements.end(); iter++)
    Requires(*iter);
}

/**
 * Adds a new child {@link Command} to the group.  The {@link Command} will be
 * started after all the previously added {@link Command Commands}.
 *
 * <p>Instead of waiting for the child to finish, a {@link CommandGroup} will
 * have it run at the same time as the subsequent {@link Command Commands}.
 * The child will run until either it finishes, a new child with conflicting
 * requirements is started, or the main sequence runs a {@link Command} with
 * conflicting requirements.  In the latter two cases, the child will be
 * canceled even if it says it can't be interrupted.</p>
 *
 * <p>Note that any requirements the given {@link Command Command} has will be
 * added to the group.  For this reason, a {@link Command Command's}
 * requirements can not be changed after being added to a group.</p>
 *
 * <p>It is recommended that this method be called in the constructor.</p>
 *
 * @param command The command to be added
 */
void CommandGroup::AddParallel(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }
  if (!AssertUnlocked("Cannot add new command to command group")) return;

  command->SetParent(this);

  m_commands.push_back(
      CommandGroupEntry(command, CommandGroupEntry::kSequence_BranchChild));
  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  Command::SubsystemSet requirements = command->GetRequirements();
  for (auto iter = requirements.begin(); iter != requirements.end(); iter++)
    Requires(*iter);
}

/**
 * Adds a new child {@link Command} to the group with the given timeout.  The
 * {@link Command} will be started after all the previously added
 * {@link Command Commands}.
 *
 * <p>Once the {@link Command Command} is started, it will run until it
 * finishes, is interrupted, or the time expires, whichever is sooner.  Note
 * that the given {@link Command Command} will have no knowledge that it is on
 * a timer.</p>
 *
 * <p>Instead of waiting for the child to finish, a {@link CommandGroup} will
 * have it run at the same time as the subsequent {@link Command Commands}.
 * The child will run until either it finishes, the timeout expires, a new
 * child with conflicting requirements is started, or the main sequence runs a
 * {@link Command} with conflicting requirements.  In the latter two cases, the
 * child will be canceled even if it says it can't be interrupted.</p>
 *
 * <p>Note that any requirements the given {@link Command Command} has will be
 * added to the group.  For this reason, a {@link Command Command's}
 * requirements can not be changed after being added to a group.</p>
 *
 * <p>It is recommended that this method be called in the constructor.</p>
 *
 * @param command The command to be added
 * @param timeout The timeout (in seconds)
 */
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

  command->SetParent(this);

  m_commands.push_back(CommandGroupEntry(
      command, CommandGroupEntry::kSequence_BranchChild, timeout));
  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  Command::SubsystemSet requirements = command->GetRequirements();
  for (auto iter = requirements.begin(); iter != requirements.end(); iter++)
    Requires(*iter);
}

void CommandGroup::_Initialize() { m_currentCommandIndex = -1; }

void CommandGroup::_Execute() {
  CommandGroupEntry entry;
  Command* cmd = nullptr;
  bool firstRun = false;

  if (m_currentCommandIndex == -1) {
    firstRun = true;
    m_currentCommandIndex = 0;
  }

  while (static_cast<size_t>(m_currentCommandIndex) < m_commands.size()) {
    if (cmd != nullptr) {
      if (entry.IsTimedOut()) cmd->_Cancel();

      if (cmd->Run()) {
        break;
      } else {
        cmd->Removed();
        m_currentCommandIndex++;
        firstRun = true;
        cmd = nullptr;
        continue;
      }
    }

    entry = m_commands[m_currentCommandIndex];
    cmd = nullptr;

    switch (entry.m_state) {
      case CommandGroupEntry::kSequence_InSequence:
        cmd = entry.m_command;
        if (firstRun) {
          cmd->StartRunning();
          CancelConflicts(cmd);
          firstRun = false;
        }
        break;

      case CommandGroupEntry::kSequence_BranchPeer:
        m_currentCommandIndex++;
        entry.m_command->Start();
        break;

      case CommandGroupEntry::kSequence_BranchChild:
        m_currentCommandIndex++;
        CancelConflicts(entry.m_command);
        entry.m_command->StartRunning();
        m_children.push_back(entry);
        break;
    }
  }

  // Run Children
  for (auto iter = m_children.begin(); iter != m_children.end();) {
    entry = *iter;
    Command* child = entry.m_command;
    if (entry.IsTimedOut()) child->_Cancel();

    if (!child->Run()) {
      child->Removed();
      iter = m_children.erase(iter);
    } else {
      iter++;
    }
  }
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

  for (auto iter = m_children.begin(); iter != m_children.end(); iter++) {
    Command* cmd = iter->m_command;
    cmd->_Cancel();
    cmd->Removed();
  }
  m_children.clear();
}

void CommandGroup::_Interrupted() { _End(); }

// Can be overwritten by teams
void CommandGroup::Initialize() {}

// Can be overwritten by teams
void CommandGroup::Execute() {}

// Can be overwritten by teams
void CommandGroup::End() {}

// Can be overwritten by teams
void CommandGroup::Interrupted() {}

bool CommandGroup::IsFinished() {
  return static_cast<size_t>(m_currentCommandIndex) >= m_commands.size() &&
         m_children.empty();
}

bool CommandGroup::IsInterruptible() const {
  if (!Command::IsInterruptible()) return false;

  if (m_currentCommandIndex != -1 &&
      static_cast<size_t>(m_currentCommandIndex) < m_commands.size()) {
    Command* cmd = m_commands[m_currentCommandIndex].m_command;
    if (!cmd->IsInterruptible()) return false;
  }

  for (auto iter = m_children.cbegin(); iter != m_children.cend(); iter++) {
    if (!iter->m_command->IsInterruptible()) return false;
  }

  return true;
}

void CommandGroup::CancelConflicts(Command* command) {
  for (auto childIter = m_children.begin(); childIter != m_children.end();) {
    Command* child = childIter->m_command;
    bool erased = false;

    Command::SubsystemSet requirements = command->GetRequirements();
    for (auto requirementIter = requirements.begin();
         requirementIter != requirements.end(); requirementIter++) {
      if (child->DoesRequire(*requirementIter)) {
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

int CommandGroup::GetSize() const { return m_children.size(); }
