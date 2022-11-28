// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandScheduler.h"

#include <cstdio>

#include <frc/RobotBase.h>
#include <frc/RobotState.h>
#include <frc/TimedRobot.h>
#include <frc/livewindow/LiveWindow.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <networktables/IntegerArrayTopic.h>
#include <networktables/NTSendableBuilder.h>
#include <networktables/StringArrayTopic.h>
#include <wpi/DenseMap.h>
#include <wpi/SmallVector.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandPtr.h"
#include "frc2/command/Subsystem.h"

using namespace frc2;

class CommandScheduler::Impl {
 public:
  // A set of the currently-running commands.
  wpi::SmallSet<Command*, 12> scheduledCommands;

  // A map from required subsystems to their requiring commands.  Also used as a
  // set of the currently-required subsystems.
  wpi::DenseMap<Subsystem*, Command*> requirements;

  // A map from subsystems registered with the scheduler to their default
  // commands.  Also used as a list of currently-registered subsystems.
  wpi::DenseMap<Subsystem*, std::unique_ptr<Command>> subsystems;

  frc::EventLoop defaultButtonLoop;
  // The set of currently-registered buttons that will be polled every
  // iteration.
  frc::EventLoop* activeButtonLoop{&defaultButtonLoop};

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
  wpi::SmallVector<Command*, 4> toSchedule;
  wpi::SmallVector<Command*, 4> toCancel;
};

template <typename TMap, typename TKey>
static bool ContainsKey(const TMap& map, TKey keyToCheck) {
  return map.find(keyToCheck) != map.end();
}

CommandScheduler::CommandScheduler()
    : m_impl(new Impl), m_watchdog(frc::TimedRobot::kDefaultPeriod, [] {
        std::puts("CommandScheduler loop time overrun.");
      }) {
  HAL_Report(HALUsageReporting::kResourceType_Command,
             HALUsageReporting::kCommand2_Scheduler);
  wpi::SendableRegistry::AddLW(this, "Scheduler");
  frc::LiveWindow::SetEnabledCallback([this] {
    this->Disable();
    this->CancelAll();
  });
  frc::LiveWindow::SetDisabledCallback([this] { this->Enable(); });
}

CommandScheduler::~CommandScheduler() {
  wpi::SendableRegistry::Remove(this);
  frc::LiveWindow::SetEnabledCallback(nullptr);
  frc::LiveWindow::SetDisabledCallback(nullptr);

  std::unique_ptr<Impl>().swap(m_impl);
}

CommandScheduler& CommandScheduler::GetInstance() {
  static CommandScheduler scheduler;
  return scheduler;
}

void CommandScheduler::SetPeriod(units::second_t period) {
  m_watchdog.SetTimeout(period);
}

frc::EventLoop* CommandScheduler::GetActiveButtonLoop() const {
  return m_impl->activeButtonLoop;
}

void CommandScheduler::SetActiveButtonLoop(frc::EventLoop* loop) {
  m_impl->activeButtonLoop = loop;
}

frc::EventLoop* CommandScheduler::GetDefaultButtonLoop() const {
  return &(m_impl->defaultButtonLoop);
}

void CommandScheduler::ClearButtons() {
  m_impl->activeButtonLoop->Clear();
}

void CommandScheduler::Schedule(Command* command) {
  if (m_impl->inRunLoop) {
    m_impl->toSchedule.emplace_back(command);
    return;
  }

  if (command->IsGrouped()) {
    throw FRC_MakeError(frc::err::CommandIllegalUse,
                        "A command that is part of a command group "
                        "cannot be independently scheduled");
    return;
  }
  if (m_impl->disabled ||
      (frc::RobotState::IsDisabled() && !command->RunsWhenDisabled()) ||
      m_impl->scheduledCommands.contains(command)) {
    return;
  }

  const auto& requirements = command->GetRequirements();

  wpi::SmallVector<Command*, 8> intersection;

  bool isDisjoint = true;
  bool allInterruptible = true;
  for (auto&& i1 : m_impl->requirements) {
    if (requirements.find(i1.first) != requirements.end()) {
      isDisjoint = false;
      allInterruptible &= (i1.second->GetInterruptionBehavior() ==
                           Command::InterruptionBehavior::kCancelSelf);
      intersection.emplace_back(i1.second);
    }
  }

  if (isDisjoint || allInterruptible) {
    if (allInterruptible) {
      for (auto&& cmdToCancel : intersection) {
        Cancel(cmdToCancel);
      }
    }
    m_impl->scheduledCommands.insert(command);
    for (auto&& requirement : requirements) {
      m_impl->requirements[requirement] = command;
    }
    command->Initialize();
    for (auto&& action : m_impl->initActions) {
      action(*command);
    }
    m_watchdog.AddEpoch(command->GetName() + ".Initialize()");
  }
}

void CommandScheduler::Schedule(std::span<Command* const> commands) {
  for (auto command : commands) {
    Schedule(command);
  }
}

void CommandScheduler::Schedule(std::initializer_list<Command*> commands) {
  for (auto command : commands) {
    Schedule(command);
  }
}

void CommandScheduler::Schedule(const CommandPtr& command) {
  Schedule(command.get());
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

  // Cache the active instance to avoid concurrency problems if SetActiveLoop()
  // is called from inside the button bindings.
  frc::EventLoop* loopCache = m_impl->activeButtonLoop;
  // Poll buttons for new commands to add.
  loopCache->Poll();
  m_watchdog.AddEpoch("buttons.Run()");

  m_impl->inRunLoop = true;
  // Run scheduled commands, remove finished commands.
  for (Command* command : m_impl->scheduledCommands) {
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

      m_impl->scheduledCommands.erase(command);
      m_watchdog.AddEpoch(command->GetName() + ".End(false)");
    }
  }
  m_impl->inRunLoop = false;

  for (Command* command : m_impl->toSchedule) {
    Schedule(command);
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
  if (m_impl->subsystems.find(subsystem) != m_impl->subsystems.end()) {
    std::puts("Tried to register an already-registered subsystem");
    return;
  }

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

void CommandScheduler::RegisterSubsystem(
    std::span<Subsystem* const> subsystems) {
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
    std::span<Subsystem* const> subsystems) {
  for (auto* subsystem : subsystems) {
    UnregisterSubsystem(subsystem);
  }
}

void CommandScheduler::SetDefaultCommand(Subsystem* subsystem,
                                         CommandPtr&& defaultCommand) {
  if (!defaultCommand.get()->HasRequirement(subsystem)) {
    throw FRC_MakeError(frc::err::CommandIllegalUse, "{}",
                        "Default commands must require their subsystem!");
  }

  SetDefaultCommandImpl(subsystem, std::move(defaultCommand).Unwrap());
}

void CommandScheduler::RemoveDefaultCommand(Subsystem* subsystem) {
  m_impl->subsystems[subsystem] = nullptr;
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
  if (!m_impl) {
    return;
  }

  if (m_impl->inRunLoop) {
    m_impl->toCancel.emplace_back(command);
    return;
  }

  auto find = m_impl->scheduledCommands.find(command);
  if (find == m_impl->scheduledCommands.end()) {
    return;
  }
  m_impl->scheduledCommands.erase(*find);
  for (auto&& requirement : m_impl->requirements) {
    if (requirement.second == command) {
      m_impl->requirements.erase(requirement.first);
    }
  }
  command->End(true);
  for (auto&& action : m_impl->interruptActions) {
    action(*command);
  }
  m_watchdog.AddEpoch(command->GetName() + ".End(true)");
}

void CommandScheduler::Cancel(const CommandPtr& command) {
  Cancel(command.get());
}

void CommandScheduler::Cancel(std::span<Command* const> commands) {
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
    commands.emplace_back(command);
  }
  Cancel(commands);
}

bool CommandScheduler::IsScheduled(
    std::span<const Command* const> commands) const {
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
  return m_impl->scheduledCommands.contains(command);
}

bool CommandScheduler::IsScheduled(const CommandPtr& command) const {
  return m_impl->scheduledCommands.contains(command.get());
}

Command* CommandScheduler::Requiring(const Subsystem* subsystem) const {
  auto find = m_impl->requirements.find(subsystem);
  if (find != m_impl->requirements.end()) {
    return find->second;
  } else {
    return nullptr;
  }
}

void CommandScheduler::Disable() {
  m_impl->disabled = true;
}

void CommandScheduler::Enable() {
  m_impl->disabled = false;
}

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

void CommandScheduler::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Scheduler");
  builder.SetUpdateTable(
      [this,
       namesPub = nt::StringArrayTopic{builder.GetTopic("Names")}.Publish(),
       idsPub = nt::IntegerArrayTopic{builder.GetTopic("Ids")}.Publish(),
       cancelEntry = nt::IntegerArrayTopic{builder.GetTopic("Cancel")}.GetEntry(
           {})]() mutable {
        auto toCancel = cancelEntry.Get();
        if (!toCancel.empty()) {
          for (auto cancel : cancelEntry.Get()) {
            uintptr_t ptrTmp = static_cast<uintptr_t>(cancel);
            Command* command = reinterpret_cast<Command*>(ptrTmp);
            if (m_impl->scheduledCommands.find(command) !=
                m_impl->scheduledCommands.end()) {
              Cancel(command);
            }
          }
          cancelEntry.Set({});
        }

        wpi::SmallVector<std::string, 8> names;
        wpi::SmallVector<int64_t, 8> ids;
        for (Command* command : m_impl->scheduledCommands) {
          names.emplace_back(command->GetName());
          uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(command);
          ids.emplace_back(static_cast<int64_t>(ptrTmp));
        }
        namesPub.Set(names);
        idsPub.Set(ids);
      });
}

void CommandScheduler::SetDefaultCommandImpl(Subsystem* subsystem,
                                             std::unique_ptr<Command> command) {
  if (command->GetInterruptionBehavior() ==
      Command::InterruptionBehavior::kCancelIncoming) {
    std::puts(
        "Registering a non-interruptible default command!\n"
        "This will likely prevent any other commands from "
        "requiring this subsystem.");
    // Warn, but allow -- there might be a use case for this.
  }
  m_impl->subsystems[subsystem] = std::move(command);
}
