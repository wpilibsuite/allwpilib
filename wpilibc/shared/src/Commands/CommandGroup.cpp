/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/CommandGroup.h"
#include "WPIErrors.h"

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

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_InSequence);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto requirement : command->GetRequirements()) Requires(requirement);
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

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_InSequence,
                          timeout);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto requirement : command->GetRequirements()) Requires(requirement);
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

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_BranchChild);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto requirement : command->GetRequirements()) Requires(requirement);
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

  m_commands.emplace_back(command, CommandGroupEntry::kSequence_BranchChild,
                          timeout);

  command->SetParent(this);

  // Iterate through command->GetRequirements() and call Requires() on each
  // required subsystem
  for (auto requirement : command->GetRequirements()) Requires(requirement);
}

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
  while ((unsigned)m_currentCommandIndex < m_commands.size()) {
    // If a command is prepared to run
    if (cmd != nullptr) {
      // If command timed out, cancel it so it's removed from the Scheduler
      if (entry->IsTimedOut()) cmd->_Cancel();

      // If command finished or was cancelled (needs removal from Scheduler)
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
  for (auto iter = m_children.begin(); iter != m_children.end();) {
    entry = *iter;
    Command* child = entry->m_command;
    if (entry->IsTimedOut()) child->_Cancel();

    /* If child finished or was cancelled, remove it from the list of children
     * to execute. Otherwise, the iterator hasn't changed so increment it to the
     * next child.
     */
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
      (unsigned)m_currentCommandIndex < m_commands.size()) {
    Command* cmd = m_commands[m_currentCommandIndex].m_command;
    cmd->_Cancel();
    cmd->Removed();
  }

  for (auto child : m_children) {
    Command* cmd = child->m_command;
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
  return (unsigned)m_currentCommandIndex >= m_commands.size() &&
         m_children.empty();
}

bool CommandGroup::IsInterruptible() const {
  if (!Command::IsInterruptible()) return false;

  if (m_currentCommandIndex != -1 &&
      (unsigned)m_currentCommandIndex < m_commands.size()) {
    Command* cmd = m_commands[m_currentCommandIndex].m_command;
    if (!cmd->IsInterruptible()) return false;
  }

  for (const auto& child : m_children) {
    if (!child->m_command->IsInterruptible()) return false;
  }

  return true;
}

void CommandGroup::CancelConflicts(Command* command) {
  for (auto childIter = m_children.begin(); childIter != m_children.end();) {
    Command* child = (*childIter)->m_command;
    bool erased = false;

    for (auto requirement : command->GetRequirements()) {
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

int CommandGroup::GetSize() const { return m_children.size(); }
