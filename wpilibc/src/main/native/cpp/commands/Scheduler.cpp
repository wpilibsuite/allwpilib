/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/Scheduler.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <hal/HAL.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/mutex.h>

#include "frc/WPIErrors.h"
#include "frc/buttons/ButtonScheduler.h"
#include "frc/commands/Command.h"
#include "frc/commands/Subsystem.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

struct Scheduler::Impl {
  void Remove(Command* command);
  void ProcessCommandAddition(Command* command);

  using SubsystemSet = std::set<Subsystem*>;
  SubsystemSet subsystems;
  wpi::mutex buttonsMutex;
  using ButtonVector = std::vector<std::unique_ptr<ButtonScheduler>>;
  ButtonVector buttons;
  using CommandVector = std::vector<Command*>;
  wpi::mutex additionsMutex;
  CommandVector additions;
  using CommandSet = std::set<Command*>;
  CommandSet commands;
  bool adding = false;
  bool enabled = true;
  std::vector<std::string> commandsBuf;
  std::vector<double> idsBuf;
  bool runningCommandsChanged = false;
};

Scheduler* Scheduler::GetInstance() {
  static Scheduler instance;
  return &instance;
}

void Scheduler::AddCommand(Command* command) {
  std::lock_guard<wpi::mutex> lock(m_impl->additionsMutex);
  if (std::find(m_impl->additions.begin(), m_impl->additions.end(), command) !=
      m_impl->additions.end())
    return;
  m_impl->additions.push_back(command);
}

void Scheduler::AddButton(ButtonScheduler* button) {
  std::lock_guard<wpi::mutex> lock(m_impl->buttonsMutex);
  m_impl->buttons.emplace_back(button);
}

void Scheduler::RegisterSubsystem(Subsystem* subsystem) {
  if (subsystem == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
    return;
  }
  m_impl->subsystems.insert(subsystem);
}

void Scheduler::Run() {
  // Get button input (going backwards preserves button priority)
  {
    if (!m_impl->enabled) return;

    std::lock_guard<wpi::mutex> lock(m_impl->buttonsMutex);
    for (auto& button : m_impl->buttons) {
      button->Execute();
    }
  }

  // Call every subsystem's periodic method
  for (auto& subsystem : m_impl->subsystems) {
    subsystem->Periodic();
  }

  m_impl->runningCommandsChanged = false;

  // Loop through the commands
  for (auto cmdIter = m_impl->commands.begin();
       cmdIter != m_impl->commands.end();) {
    Command* command = *cmdIter;
    // Increment before potentially removing to keep the iterator valid
    ++cmdIter;
    if (!command->Run()) {
      Remove(command);
      m_impl->runningCommandsChanged = true;
    }
  }

  // Add the new things
  {
    std::lock_guard<wpi::mutex> lock(m_impl->additionsMutex);
    for (auto& addition : m_impl->additions) {
      // Check to make sure no adding during adding
      if (m_impl->adding) {
        wpi_setWPIErrorWithContext(IncompatibleState,
                                   "Can not start command from cancel method");
      } else {
        m_impl->ProcessCommandAddition(addition);
      }
    }
    m_impl->additions.clear();
  }

  // Add in the defaults
  for (auto& subsystem : m_impl->subsystems) {
    if (subsystem->GetCurrentCommand() == nullptr) {
      if (m_impl->adding) {
        wpi_setWPIErrorWithContext(IncompatibleState,
                                   "Can not start command from cancel method");
      } else {
        m_impl->ProcessCommandAddition(subsystem->GetDefaultCommand());
      }
    }
    subsystem->ConfirmCommand();
  }
}

void Scheduler::Remove(Command* command) {
  if (command == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "command");
    return;
  }

  m_impl->Remove(command);
}

void Scheduler::RemoveAll() {
  while (m_impl->commands.size() > 0) {
    Remove(*m_impl->commands.begin());
  }
}

void Scheduler::ResetAll() {
  RemoveAll();
  m_impl->subsystems.clear();
  m_impl->buttons.clear();
  m_impl->additions.clear();
  m_impl->commands.clear();
}

void Scheduler::SetEnabled(bool enabled) { m_impl->enabled = enabled; }

void Scheduler::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Scheduler");
  auto namesEntry = builder.GetEntry("Names");
  auto idsEntry = builder.GetEntry("Ids");
  auto cancelEntry = builder.GetEntry("Cancel");
  builder.SetUpdateTable([=]() {
    // Get the list of possible commands to cancel
    auto new_toCancel = cancelEntry.GetValue();
    wpi::ArrayRef<double> toCancel;
    if (new_toCancel) toCancel = new_toCancel->GetDoubleArray();

    // Cancel commands whose cancel buttons were pressed on the SmartDashboard
    if (!toCancel.empty()) {
      for (auto& command : m_impl->commands) {
        for (const auto& cancelled : toCancel) {
          if (command->GetID() == cancelled) {
            command->Cancel();
          }
        }
      }
      nt::NetworkTableEntry(cancelEntry).SetDoubleArray({});
    }

    // Set the running commands
    if (m_impl->runningCommandsChanged) {
      m_impl->commandsBuf.resize(0);
      m_impl->idsBuf.resize(0);
      for (const auto& command : m_impl->commands) {
        m_impl->commandsBuf.emplace_back(command->GetName());
        m_impl->idsBuf.emplace_back(command->GetID());
      }
      nt::NetworkTableEntry(namesEntry).SetStringArray(m_impl->commandsBuf);
      nt::NetworkTableEntry(idsEntry).SetDoubleArray(m_impl->idsBuf);
    }
  });
}

Scheduler::Scheduler() : m_impl(new Impl) {
  HAL_Report(HALUsageReporting::kResourceType_Command,
             HALUsageReporting::kCommand_Scheduler);
  SetName("Scheduler");
}

Scheduler::~Scheduler() {}

void Scheduler::Impl::Remove(Command* command) {
  if (!commands.erase(command)) return;

  for (auto&& requirement : command->GetRequirements()) {
    requirement->SetCurrentCommand(nullptr);
  }

  command->Removed();
}

void Scheduler::Impl::ProcessCommandAddition(Command* command) {
  if (command == nullptr) return;

  // Only add if not already in
  auto found = commands.find(command);
  if (found == commands.end()) {
    // Check that the requirements can be had
    const auto& requirements = command->GetRequirements();
    for (const auto& requirement : requirements) {
      if (requirement->GetCurrentCommand() != nullptr &&
          !requirement->GetCurrentCommand()->IsInterruptible())
        return;
    }

    // Give it the requirements
    adding = true;
    for (auto&& requirement : requirements) {
      if (requirement->GetCurrentCommand() != nullptr) {
        requirement->GetCurrentCommand()->Cancel();
        Remove(requirement->GetCurrentCommand());
      }
      requirement->SetCurrentCommand(command);
    }
    adding = false;

    commands.insert(command);

    command->StartRunning();
    runningCommandsChanged = true;
  }
}
