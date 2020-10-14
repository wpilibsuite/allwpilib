/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/CommandScheduler.h"

#include <frc/RobotBase.h>
#include <frc/RobotState.h>
#include <frc/TimedRobot.h>
#include <frc/WPIErrors.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/smartdashboard/SendableBuilder.h>
#include <frc/smartdashboard/SendableRegistry.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/DenseMap.h>
#include <wpi/SmallVector.h>
#include <wpi/raw_ostream.h>

#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandState.h"
#include "frc2/command/Subsystem.h"

using namespace frc2;

class CommandScheduler::Impl {
 public:
  // A map from commands to their scheduling state.  Also used as a set of the
  // currently-running commands.
  wpi::DenseMap<Command*, CommandState> scheduledCommands;

  // A map from required subsystems to their requiring commands.  Also used as a
  // set of the currently-required subsystems.
  wpi::DenseMap<Subsystem*, Command*> requirements;

  // A map from subsystems registered with the scheduler to their default
  // commands.  Also used as a list of currently-registered subsystems.
  wpi::DenseMap<Subsystem*, std::unique_ptr<Command>> subsystems;

  // The set of currently-registered buttons that will be polled every
  // iteration.
  wpi::SmallVector<wpi::unique_function<void()>, 4> buttons;

  bool disabled{false};

  // Lists of user-supplied actions to be executed on scheduling events for
  // every command.
  wpi::SmallVector<Action, 4> initActions;
  wpi::SmallVector<Action, 4> executeActions;
  wpi::SmallVector<Action, 4> interruptActions;
  wpi::SmallVector<Action, 4> finishActions;

  // Flag and queues for avoiding concurrent modification if commands are
  // scheduled/canceled during run

  bool inRunLoop = false;
  wpi::DenseMap<Command*, bool> toSchedule;
  wpi::SmallVector<Command*, 4> toCancel;
};

template <typename TMap, typename TKey>
static bool ContainsKey(const TMap& map, TKey keyToCheck) {
  return map.find(keyToCheck) != map.end();
}

CommandScheduler::CommandScheduler()
    : m_impl(new Impl), m_watchdog(frc::TimedRobot::kDefaultPeriod, [] {
        wpi::outs() << "CommandScheduler loop time overrun.\n";
      }) {
  HAL_Report(HALUsageReporting::kResourceType_Command,
             HALUsageReporting::kCommand2_Scheduler);
  frc::SendableRegistry::GetInstance().AddLW(this, "Scheduler");
  auto scheduler = frc::LiveWindow::GetInstance();
  scheduler->enabled = [this] {
    this->Disable();
    this->CancelAll();
  };
  scheduler->disabled = [this] { this->Enable(); };
}

CommandScheduler::~CommandScheduler() {
  frc::SendableRegistry::GetInstance().Remove(this);
  auto scheduler = frc::LiveWindow::GetInstance();
  scheduler->enabled = nullptr;
  scheduler->disabled = nullptr;
}

CommandScheduler& CommandScheduler::GetInstance() {
  static CommandScheduler scheduler;
  return scheduler;
}

void CommandScheduler::SetPeriod(units::second_t period) {
  m_watchdog.SetTimeout(period);
}

void CommandScheduler::AddButton(wpi::unique_function<void()> button) {
  m_impl->buttons.emplace_back(std::move(button));
}

void CommandScheduler::ClearButtons() { m_impl->buttons.clear(); }

void CommandScheduler::Schedule(bool interruptible, Command* command) {
  if (m_impl->inRunLoop) {
    m_impl->toSchedule.try_emplace(command, interruptible);
    return;
  }

  if (command->IsGrouped()) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "A command that is part of a command group "
                               "cannot be independently scheduled");
    return;
  }
  if (m_impl->disabled ||
      (frc::RobotState::IsDisabled() && !command->RunsWhenDisabled()) ||
      ContainsKey(m_impl->scheduledCommands, command)) {
    return;
  }

  const auto& requirements = command->GetRequirements();

  wpi::SmallVector<Command*, 8> intersection;

  bool isDisjoint = true;
  bool allInterruptible = true;
  for (auto&& i1 : m_impl->requirements) {
    if (requirements.find(i1.first) != requirements.end()) {
      isDisjoint = false;
      allInterruptible &=
          m_impl->scheduledCommands[i1.second].IsInterruptible();
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
    m_impl->scheduledCommands[command] = CommandState{interruptible};
    for (auto&& requirement : requirements) {
      m_impl->requirements[requirement] = command;
    }
    for (auto&& action : m_impl->initActions) {
      action(*command);
    }
    m_watchdog.AddEpoch(command->GetName() + ".Initialize()");
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
  if (m_impl->disabled) {
    return;
  }

  m_watchdog.Reset();

  // Run the periodic method of all registered subsystems.
  for (auto&& subsystem : m_impl->subsystems) {
    subsystem.getFirst()->Periodic();
    if constexpr (frc::RobotBase::IsSimulation()) {
      subsystem.getFirst()->SimulationPeriodic();
    }
    m_watchdog.AddEpoch("Subsystem Periodic()");
  }

  // Poll buttons for new commands to add.
  for (auto&& button : m_impl->buttons) {
    button();
  }
  m_watchdog.AddEpoch("buttons.Run()");

  m_impl->inRunLoop = true;
  // Run scheduled commands, remove finished commands.
  for (auto iterator = m_impl->scheduledCommands.begin();
       iterator != m_impl->scheduledCommands.end(); iterator++) {
    Command* command = iterator->getFirst();

    if (!command->RunsWhenDisabled() && frc::RobotState::IsDisabled()) {
      Cancel(command);
      continue;
    }

    command->Execute();
    for (auto&& action : m_impl->executeActions) {
      action(*command);
    }
    m_watchdog.AddEpoch(command->GetName() + ".Execute()");

    if (command->IsFinished()) {
      command->End(false);
      for (auto&& action : m_impl->finishActions) {
        action(*command);
      }

      for (auto&& requirement : command->GetRequirements()) {
        m_impl->requirements.erase(requirement);
      }

      m_impl->scheduledCommands.erase(iterator);
      m_watchdog.AddEpoch(command->GetName() + ".End(false)");
    }
  }
  m_impl->inRunLoop = false;

  for (auto&& commandInterruptible : m_impl->toSchedule) {
    Schedule(commandInterruptible.second, commandInterruptible.first);
  }

  for (auto&& command : m_impl->toCancel) {
    Cancel(command);
  }

  m_impl->toSchedule.clear();
  m_impl->toCancel.clear();

  // Add default commands for un-required registered subsystems.
  for (auto&& subsystem : m_impl->subsystems) {
    auto s = m_impl->requirements.find(subsystem.getFirst());
    if (s == m_impl->requirements.end() && subsystem.getSecond()) {
      Schedule({subsystem.getSecond().get()});
    }
  }

  m_watchdog.Disable();
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
}

void CommandScheduler::RegisterSubsystem(Subsystem* subsystem) {
  m_impl->subsystems[subsystem] = nullptr;
}

void CommandScheduler::UnregisterSubsystem(Subsystem* subsystem) {
  auto s = m_impl->subsystems.find(subsystem);
  if (s != m_impl->subsystems.end()) {
    m_impl->subsystems.erase(s);
  }
}

void CommandScheduler::RegisterSubsystem(
    std::initializer_list<Subsystem*> subsystems) {
  for (auto* subsystem : subsystems) {
    RegisterSubsystem(subsystem);
  }
}

void CommandScheduler::RegisterSubsystem(wpi::ArrayRef<Subsystem*> subsystems) {
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

void CommandScheduler::UnregisterSubsystem(
    wpi::ArrayRef<Subsystem*> subsystems) {
  for (auto* subsystem : subsystems) {
    UnregisterSubsystem(subsystem);
  }
}

Command* CommandScheduler::GetDefaultCommand(const Subsystem* subsystem) const {
  auto&& find = m_impl->subsystems.find(subsystem);
  if (find != m_impl->subsystems.end()) {
    return find->second.get();
  } else {
    return nullptr;
  }
}

void CommandScheduler::Cancel(Command* command) {
  if (m_impl->inRunLoop) {
    m_impl->toCancel.emplace_back(command);
    return;
  }

  auto find = m_impl->scheduledCommands.find(command);
  if (find == m_impl->scheduledCommands.end()) return;
  command->End(true);
  for (auto&& action : m_impl->interruptActions) {
    action(*command);
  }
  m_watchdog.AddEpoch(command->GetName() + ".End(true)");
  m_impl->scheduledCommands.erase(find);
  for (auto&& requirement : m_impl->requirements) {
    if (requirement.second == command) {
      m_impl->requirements.erase(requirement.first);
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
  wpi::SmallVector<Command*, 16> commands;
  for (auto&& command : m_impl->scheduledCommands) {
    commands.emplace_back(command.first);
  }
  Cancel(commands);
}

double CommandScheduler::TimeSinceScheduled(const Command* command) const {
  auto find = m_impl->scheduledCommands.find(command);
  if (find != m_impl->scheduledCommands.end()) {
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
  return m_impl->scheduledCommands.find(command) !=
         m_impl->scheduledCommands.end();
}

Command* CommandScheduler::Requiring(const Subsystem* subsystem) const {
  auto find = m_impl->requirements.find(subsystem);
  if (find != m_impl->requirements.end()) {
    return find->second;
  } else {
    return nullptr;
  }
}

void CommandScheduler::Disable() { m_impl->disabled = true; }

void CommandScheduler::Enable() { m_impl->disabled = false; }

void CommandScheduler::OnCommandInitialize(Action action) {
  m_impl->initActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandExecute(Action action) {
  m_impl->executeActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandInterrupt(Action action) {
  m_impl->interruptActions.emplace_back(std::move(action));
}

void CommandScheduler::OnCommandFinish(Action action) {
  m_impl->finishActions.emplace_back(std::move(action));
}

void CommandScheduler::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Scheduler");
  auto namesEntry = builder.GetEntry("Names");
  auto idsEntry = builder.GetEntry("Ids");
  auto cancelEntry = builder.GetEntry("Cancel");

  builder.SetUpdateTable([=] {
    double tmp[1];
    tmp[0] = 0;
    auto toCancel = cancelEntry.GetDoubleArray(tmp);
    for (auto cancel : toCancel) {
      uintptr_t ptrTmp = static_cast<uintptr_t>(cancel);
      Command* command = reinterpret_cast<Command*>(ptrTmp);
      if (m_impl->scheduledCommands.find(command) !=
          m_impl->scheduledCommands.end()) {
        Cancel(command);
      }
      nt::NetworkTableEntry(cancelEntry)
          .SetDoubleArray(wpi::ArrayRef<double>{});
    }

    wpi::SmallVector<std::string, 8> names;
    wpi::SmallVector<double, 8> ids;
    for (auto&& command : m_impl->scheduledCommands) {
      names.emplace_back(command.first->GetName());
      uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(command.first);
      ids.emplace_back(static_cast<double>(ptrTmp));
    }
    nt::NetworkTableEntry(namesEntry).SetStringArray(names);
    nt::NetworkTableEntry(idsEntry).SetDoubleArray(ids);
  });
}

void CommandScheduler::SetDefaultCommandImpl(Subsystem* subsystem,
                                             std::unique_ptr<Command> command) {
  m_impl->subsystems[subsystem] = std::move(command);
}
