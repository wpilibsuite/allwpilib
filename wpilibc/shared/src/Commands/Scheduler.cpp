/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Scheduler.h"

#include <algorithm>
#include <iostream>
#include <set>
#include "Buttons/ButtonScheduler.h"
#include "Commands/Subsystem.h"
#include "HLUsageReporting.h"
#include "WPIErrors.h"

Scheduler::Scheduler() { HLUsageReporting::ReportScheduler(); }

/**
 * Returns the {@link Scheduler}, creating it if one does not exist.
 *
 * @return the {@link Scheduler}
 */
Scheduler* Scheduler::GetInstance() {
  static Scheduler instance;
  return &instance;
}

void Scheduler::SetEnabled(bool enabled) { m_enabled = enabled; }

/**
 * Add a command to be scheduled later.
 *
 * In any pass through the scheduler, all commands are added to the additions
 * list, then at the end of the pass, they are all scheduled.
 *
 * @param command The command to be scheduled
 */
void Scheduler::AddCommand(Command* command) {
  std::lock_guard<priority_mutex> sync(m_additionsLock);
  if (std::find(m_additions.begin(), m_additions.end(), command) !=
      m_additions.end())
    return;
  m_additions.push_back(command);
}

void Scheduler::AddButton(ButtonScheduler* button) {
  std::lock_guard<priority_mutex> sync(m_buttonsLock);
  m_buttons.emplace_back(button);
}

void Scheduler::ProcessCommandAddition(Command* command) {
  if (command == nullptr) return;

  // Check to make sure no adding during adding
  if (m_adding) {
    wpi_setWPIErrorWithContext(IncompatibleState,
                               "Can not start command from cancel method");
    return;
  }

  // Only add if not already in
  auto found = m_commands.find(command);
  if (found == m_commands.end()) {
    // Check that the requirements can be had
    for (auto requirement : command->GetRequirements()) {
      if (requirement->GetCurrentCommand() != nullptr &&
          !requirement->GetCurrentCommand()->IsInterruptible())
        return;
    }

    // Give it the requirements
    m_adding = true;
    for (auto requirement : command->GetRequirements()) {
      if (requirement->GetCurrentCommand() != nullptr) {
        requirement->GetCurrentCommand()->Cancel();
        Remove(requirement->GetCurrentCommand());
      }
      requirement->SetCurrentCommand(command);
    }
    m_adding = false;

    m_commands.insert(command);

    command->StartRunning();
    m_runningCommandsChanged = true;
  }
}

/**
 * Runs a single iteration of the loop.
 *
 * This method should be called often in order to have a functioning
 * {@link Command} system.  The loop has five stages:
 *
 * <ol>
 * <li> Poll the Buttons </li>
 * <li> Execute/Remove the Commands </li>
 * <li> Send values to SmartDashboard </li>
 * <li> Add Commands </li>
 * <li> Add Defaults </li>
 * </ol>
 */
void Scheduler::Run() {
  // Get button input (going backwards preserves button priority)
  {
    if (!m_enabled) return;

    std::lock_guard<priority_mutex> sync(m_buttonsLock);
    for (auto& button : m_buttons) {
      button->Execute();
    }
  }

  m_runningCommandsChanged = false;

  // Loop through the commands
  for (auto cmdIter = m_commands.begin(); cmdIter != m_commands.end();) {
    Command* command = *cmdIter;
    // Increment before potentially removing to keep the iterator valid
    cmdIter++;
    if (!command->Run()) {
      Remove(command);
      m_runningCommandsChanged = true;
    }
  }

  // Add the new things
  {
    std::lock_guard<priority_mutex> sync(m_additionsLock);
    for (auto addition : m_additions) {
      ProcessCommandAddition(addition);
    }
    m_additions.clear();
  }

  // Add in the defaults
  for (auto subsystem : m_subsystems) {
    if (subsystem->GetCurrentCommand() == nullptr) {
      ProcessCommandAddition(subsystem->GetDefaultCommand());
    }
    subsystem->ConfirmCommand();
  }

  UpdateTable();
}

/**
 * Registers a {@link Subsystem} to this {@link Scheduler}, so that the {@link
 * Scheduler} might know if a default {@link Command} needs to be run.
 *
 * All {@link Subsystem Subsystems} should call this.
 *
 * @param system the system
 */
void Scheduler::RegisterSubsystem(Subsystem* subsystem) {
  if (subsystem == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
    return;
  }
  m_subsystems.insert(subsystem);
}

/**
 * Removes the {@link Command} from the {@link Scheduler}.
 *
 * @param command the command to remove
 */
void Scheduler::Remove(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }

  if (!m_commands.erase(command)) return;

  for (auto requirement : command->GetRequirements()) {
    requirement->SetCurrentCommand(nullptr);
  }

  command->Removed();
}

void Scheduler::RemoveAll() {
  while (m_commands.size() > 0) {
    Remove(*m_commands.begin());
  }
}

/**
 * Completely resets the scheduler. Undefined behavior if running.
 */
void Scheduler::ResetAll() {
  RemoveAll();
  m_subsystems.clear();
  m_buttons.clear();
  m_additions.clear();
  m_commands.clear();
  m_table = nullptr;
}

/**
 * Update the network tables associated with the Scheduler object on the
 * SmartDashboard.
 */
void Scheduler::UpdateTable() {
  if (m_table != nullptr) {
    // Get the list of possible commands to cancel
    auto new_toCancel = m_table->GetValue("Cancel");
    if (new_toCancel)
      toCancel = new_toCancel->GetDoubleArray();
    else
      toCancel.resize(0);

    /*
     * Cancel commands that have had the cancel buttons pressed on the
     * SmartDashboard
     */
    if (!toCancel.empty()) {
      for (auto command : m_commands) {
        for (auto& cancelled : toCancel) {
          if (command->GetID() == cancelled) {
            command->Cancel();
          }
        }
      }
      toCancel.resize(0);
      m_table->PutValue("Cancel", nt::Value::MakeDoubleArray(toCancel));
    }

    // Set the running commands
    if (m_runningCommandsChanged) {
      commands.resize(0);
      ids.resize(0);
      for (auto command : m_commands) {
        commands.emplace_back(command->GetName());
        ids.emplace_back(command->GetID());
      }
      m_table->PutValue("Names", nt::Value::MakeStringArray(commands));
      m_table->PutValue("Ids", nt::Value::MakeDoubleArray(ids));
    }
  }
}

std::string Scheduler::GetName() const { return "Scheduler"; }

std::string Scheduler::GetType() const { return "Scheduler"; }

std::string Scheduler::GetSmartDashboardType() const { return "Scheduler"; }

void Scheduler::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;

  m_table->PutValue("Names", nt::Value::MakeStringArray(commands));
  m_table->PutValue("Ids", nt::Value::MakeDoubleArray(ids));
  m_table->PutValue("Cancel", nt::Value::MakeDoubleArray(toCancel));
}

std::shared_ptr<ITable> Scheduler::GetTable() const { return m_table; }
