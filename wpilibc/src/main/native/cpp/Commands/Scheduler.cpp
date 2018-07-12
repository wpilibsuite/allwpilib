/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Scheduler.h"

#include <algorithm>
#include <set>

#include <HAL/HAL.h>

#include "Buttons/ButtonScheduler.h"
#include "Commands/Subsystem.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

Scheduler* Scheduler::GetInstance() {
  static Scheduler instance;
  return &instance;
}

void Scheduler::AddCommand(Command* command) {
  std::lock_guard<wpi::mutex> lock(m_additionsMutex);
  if (std::find(m_additions.begin(), m_additions.end(), command) !=
      m_additions.end())
    return;
  m_additions.push_back(command);
}

void Scheduler::AddButton(ButtonScheduler* button) {
  std::lock_guard<wpi::mutex> lock(m_buttonsMutex);
  m_buttons.emplace_back(button);
}

void Scheduler::RegisterSubsystem(Subsystem* subsystem) {
  if (subsystem == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
    return;
  }
  m_subsystems.insert(subsystem);
}

void Scheduler::Run() {
  // Get button input (going backwards preserves button priority)
  {
    if (!m_enabled) return;

    std::lock_guard<wpi::mutex> lock(m_buttonsMutex);
    for (auto& button : m_buttons) {
      button->Execute();
    }
  }

  // Call every subsystem's periodic method
  for (auto& subsystem : m_subsystems) {
    subsystem->Periodic();
  }

  m_runningCommandsChanged = false;

  // Loop through the commands
  for (auto cmdIter = m_commands.begin(); cmdIter != m_commands.end();) {
    Command* command = *cmdIter;
    // Increment before potentially removing to keep the iterator valid
    ++cmdIter;
    if (!command->Run()) {
      Remove(command);
      m_runningCommandsChanged = true;
    }
  }

  // Add the new things
  {
    std::lock_guard<wpi::mutex> lock(m_additionsMutex);
    for (auto& addition : m_additions) {
      ProcessCommandAddition(addition);
    }
    m_additions.clear();
  }

  // Add in the defaults
  for (auto& subsystem : m_subsystems) {
    if (subsystem->GetCurrentCommand() == nullptr) {
      ProcessCommandAddition(subsystem->GetDefaultCommand());
    }
    subsystem->ConfirmCommand();
  }
}

void Scheduler::Remove(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }

  if (!m_commands.erase(command)) return;

  for (auto& requirement : command->GetRequirements()) {
    requirement->SetCurrentCommand(nullptr);
  }

  command->Removed();
}

void Scheduler::RemoveAll() {
  while (m_commands.size() > 0) {
    Remove(*m_commands.begin());
  }
}

void Scheduler::ResetAll() {
  RemoveAll();
  m_subsystems.clear();
  m_buttons.clear();
  m_additions.clear();
  m_commands.clear();
  m_namesEntry = nt::NetworkTableEntry();
  m_idsEntry = nt::NetworkTableEntry();
  m_cancelEntry = nt::NetworkTableEntry();
}

void Scheduler::SetEnabled(bool enabled) { m_enabled = enabled; }

void Scheduler::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Scheduler");
  m_namesEntry = builder.GetEntry("Names");
  m_idsEntry = builder.GetEntry("Ids");
  m_cancelEntry = builder.GetEntry("Cancel");
  builder.SetUpdateTable([=]() {
    // Get the list of possible commands to cancel
    auto new_toCancel = m_cancelEntry.GetValue();
    if (new_toCancel)
      toCancel = new_toCancel->GetDoubleArray();
    else
      toCancel.resize(0);

    // Cancel commands whose cancel buttons were pressed on the SmartDashboard
    if (!toCancel.empty()) {
      for (auto& command : m_commands) {
        for (const auto& cancelled : toCancel) {
          if (command->GetID() == cancelled) {
            command->Cancel();
          }
        }
      }
      toCancel.resize(0);
      m_cancelEntry.SetDoubleArray(toCancel);
    }

    // Set the running commands
    if (m_runningCommandsChanged) {
      commands.resize(0);
      ids.resize(0);
      for (const auto& command : m_commands) {
        commands.emplace_back(command->GetName());
        ids.emplace_back(command->GetID());
      }
      m_namesEntry.SetStringArray(commands);
      m_idsEntry.SetDoubleArray(ids);
    }
  });
}

Scheduler::Scheduler() {
  HAL_Report(HALUsageReporting::kResourceType_Command,
             HALUsageReporting::kCommand_Scheduler);
  SetName("Scheduler");
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
    Command::SubsystemSet requirements = command->GetRequirements();
    for (const auto& requirement : requirements) {
      if (requirement->GetCurrentCommand() != nullptr &&
          !requirement->GetCurrentCommand()->IsInterruptible())
        return;
    }

    // Give it the requirements
    m_adding = true;
    for (auto& requirement : requirements) {
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
