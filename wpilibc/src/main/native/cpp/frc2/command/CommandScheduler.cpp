/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/CommandScheduler.h"

#include <frc/RobotState.h>
#include <frc/WPIErrors.h>
#include <frc/commands/Scheduler.h>
#include <frc/smartdashboard/SendableBuilder.h>
#include <frc/smartdashboard/SendableRegistry.h>
#include <frc2/command/CommandGroupBase.h>
#include <frc2/command/Subsystem.h>

#include <hal/HAL.h>

using namespace frc2;
template <typename TMap, typename TKey>
static bool ContainsKey(const TMap& map, TKey keyToCheck) {
  return map.find(keyToCheck) != map.end();
}

CommandScheduler::CommandScheduler() {
  frc::SendableRegistry::GetInstance().AddLW(this, "Scheduler");
}

CommandScheduler& CommandScheduler::GetInstance() {
  static CommandScheduler scheduler;
  return scheduler;
}

void CommandScheduler::AddButton(wpi::unique_function<void()> button) {
  m_buttons.emplace_back(std::move(button));
}

void CommandScheduler::ClearButtons() { m_buttons.clear(); }

void CommandScheduler::Schedule(bool interruptible, Command* command) {
  if (command->IsGrouped()) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "A command that is part of a command group "
                               "cannot be independently scheduled");
    return;
  }
  if (m_disabled ||
      (frc::RobotState::IsDisabled() && !command->RunsWhenDisabled()) ||
      ContainsKey(m_scheduledCommands, command)) {
    return;
  }

  const auto& requirements = command->GetRequirements();

  wpi::SmallVector<Command*, 8> intersection;

  bool isDisjoint = true;
  bool allInterruptible = true;
  for (auto&& i1 : m_requirements) {
    if (requirements.find(i1.first) != requirements.end()) {
      isDisjoint = false;
      allInterruptible &= m_scheduledCommands[i1.second].IsInterruptible();
      intersection.emplace_back(i1.second);
    }
  }

  if (isDisjoint || allInterruptible) {
    if (allInterruptible) {
      for (auto&& cmdToCancel : intersection) {
        Cancel(cmdToCancel);
      }
    }
    command->Initialize();
    m_scheduledCommands[command] = CommandState{interruptible};
    for (auto&& action : m_initActions) {
      action(*command);
    }
    for (auto&& requirement : requirements) {
      m_requirements[requirement] = command;
    }
  }
}

void CommandScheduler::Schedule(Command* command) { Schedule(true, command); }

void CommandScheduler::Schedule(bool interruptible,
                                wpi::ArrayRef<Command*> commands) {
  for (auto command : commands) {
    Schedule(interruptible, command);
  }
}

void CommandScheduler::Schedule(bool interruptible,
                                std::initializer_list<Command*> commands) {
  for (auto command : commands) {
    Schedule(interruptible, command);
  }
}

void CommandScheduler::Schedule(wpi::ArrayRef<Command*> commands) {
  for (auto command : commands) {
    Schedule(true, command);
  }
}

void CommandScheduler::Schedule(std::initializer_list<Command*> commands) {
  for (auto command : commands) {
    Schedule(true, command);
  }
}

void CommandScheduler::Run() {
  if (m_disabled) {
    return;
  }

  // Run the periodic method of all registered subsystems.
  for (auto&& subsystem : m_subsystems) {
    subsystem.getFirst()->Periodic();
  }

  // Poll buttons for new commands to add.
  for (auto&& button : m_buttons) {
    button();
  }

  // Run scheduled commands, remove finished commands.
  for (auto iterator = m_scheduledCommands.begin();
       iterator != m_scheduledCommands.end(); iterator++) {
    Command* command = iterator->getFirst();

    if (!command->RunsWhenDisabled() && frc::RobotState::IsDisabled()) {
      Cancel(command);
      continue;
    }

    command->Execute();
    for (auto&& action : m_executeActions) {
      action(*command);
    }

    if (command->IsFinished()) {
      command->End(false);
      for (auto&& action : m_finishActions) {
        action(*command);
      }

      for (auto&& requirement : command->GetRequirements()) {
        m_requirements.erase(requirement);
      }

      m_scheduledCommands.erase(iterator);
    }
  }

  // Add default commands for un-required registered subsystems.
  for (auto&& subsystem : m_subsystems) {
    auto s = m_requirements.find(subsystem.getFirst());
    if (s == m_requirements.end()) {
      Schedule({subsystem.getSecond().get()});
    }
  }
}

void CommandScheduler::RegisterSubsystem(Subsystem* subsystem) {
  m_subsystems[subsystem] = nullptr;
}

void CommandScheduler::UnregisterSubsystem(Subsystem* subsystem) {
  auto s = m_subsystems.find(subsystem);
  if (s != m_subsystems.end()) {
    m_subsystems.erase(s);
  }
}

void CommandScheduler::RegisterSubsystem(
    std::initializer_list<Subsystem*> subsystems) {
  for (auto* subsystem : subsystems) {
    RegisterSubsystem(subsystem);
  }
}

void CommandScheduler::UnregisterSubsystem(
    std::initializer_list<Subsystem*> subsystems) {
  for (auto* subsystem : subsystems) {
    UnregisterSubsystem(subsystem);
  }
}

Command* CommandScheduler::GetDefaultCommand(const Subsystem* subsystem) const {
  auto&& find = m_subsystems.find(subsystem);
  if (find != m_subsystems.end()) {
    return find->second.get();
  } else {
    return nullptr;
  }
}

void CommandScheduler::Cancel(Command* command) {
  auto find = m_scheduledCommands.find(command);
  if (find == m_scheduledCommands.end()) return;
  command->End(true);
  for (auto&& action : m_interruptActions) {
    action(*command);
  }
  m_scheduledCommands.erase(find);
  for (auto&& requirement : m_requirements) {
    if (requirement.second == command) {
      m_requirements.erase(requirement.first);
    }
  }
}

void CommandScheduler::Cancel(wpi::ArrayRef<Command*> commands) {
  for (auto command : commands) {
    Cancel(command);
  }
}

void CommandScheduler::Cancel(std::initializer_list<Command*> commands) {
  for (auto command : commands) {
    Cancel(command);
  }
}

void CommandScheduler::CancelAll() {
  for (auto&& command : m_scheduledCommands) {
    Cancel(command.first);
  }
}

double CommandScheduler::TimeSinceScheduled(const Command* command) const {
  auto find = m_scheduledCommands.find(command);
  if (find != m_scheduledCommands.end()) {
    return find->second.TimeSinceInitialized();
  } else {
    return -1;
  }
}
bool CommandScheduler::IsScheduled(
    wpi::ArrayRef<const Command*> commands) const {
  for (auto command : commands) {
    if (!IsScheduled(command)) {
      return false;
    }
  }
  return true;
}

bool CommandScheduler::IsScheduled(
    std::initializer_list<const Command*> commands) const {
  for (auto command : commands) {
    if (!IsScheduled(command)) {
      return false;
    }
  }
  return true;
}

bool CommandScheduler::IsScheduled(const Command* command) const {
  return m_scheduledCommands.find(command) != m_scheduledCommands.end();
}

Command* CommandScheduler::Requiring(const Subsystem* subsystem) const {
  auto find = m_requirements.find(subsystem);
  if (find != m_requirements.end()) {
    return find->second;
  } else {
    return nullptr;
  }
}

void CommandScheduler::Disable() { m_disabled = true; }

void CommandScheduler::Enable() { m_disabled = false; }

void CommandScheduler::OnCommandInitialize(Action action) {
  m_initActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandExecute(Action action) {
  m_executeActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandInterrupt(Action action) {
  m_interruptActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandFinish(Action action) {
  m_finishActions.emplace_back(std::move(action));
}

void CommandScheduler::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Scheduler");
  m_namesEntry = builder.GetEntry("Names");
  m_idsEntry = builder.GetEntry("Ids");
  m_cancelEntry = builder.GetEntry("Cancel");

  builder.SetUpdateTable([this] {
    double tmp[1];
    tmp[0] = 0;
    auto toCancel = m_cancelEntry.GetDoubleArray(tmp);
    for (auto cancel : toCancel) {
      uintptr_t ptrTmp = static_cast<uintptr_t>(cancel);
      Command* command = reinterpret_cast<Command*>(ptrTmp);
      if (m_scheduledCommands.find(command) != m_scheduledCommands.end()) {
        Cancel(command);
      }
      m_cancelEntry.SetDoubleArray(wpi::ArrayRef<double>{});
    }

    wpi::SmallVector<std::string, 8> names;
    wpi::SmallVector<double, 8> ids;
    for (auto&& command : m_scheduledCommands) {
      names.emplace_back(command.first->GetName());
      uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(command.first);
      ids.emplace_back(static_cast<double>(ptrTmp));
    }
    m_namesEntry.SetStringArray(names);
    m_idsEntry.SetDoubleArray(ids);
  });
}
