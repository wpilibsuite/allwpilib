// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Command.h"

#include "frc2/command/CommandHelper.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/EndlessCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PerpetualCommand.h"
#include "frc2/command/ProxyScheduleCommand.h"
#include "frc2/command/RepeatCommand.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/WrapperCommand.h"

using namespace frc2;

Command::~Command() {
  CommandScheduler::GetInstance().Cancel(this);
}

Command& Command::operator=(const Command& rhs) {
  m_isGrouped = false;
  return *this;
}

void Command::Initialize() {}
void Command::Execute() {}
void Command::End(bool interrupted) {}

ParallelRaceGroup Command::WithTimeout(units::second_t duration) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitCommand>(duration));
  temp.emplace_back(std::move(*this).TransferOwnership());
  return ParallelRaceGroup(std::move(temp));
}

ParallelRaceGroup Command::Until(std::function<bool()> condition) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitUntilCommand>(std::move(condition)));
  temp.emplace_back(std::move(*this).TransferOwnership());
  return ParallelRaceGroup(std::move(temp));
}

std::unique_ptr<Command> Command::WithRunsWhenDisabled(
    bool doesRunWhenDisabled) && {
  class RunsWhenDisabledCommand
      : public CommandHelper<WrapperCommand, RunsWhenDisabledCommand> {
   public:
    RunsWhenDisabledCommand(std::unique_ptr<Command>&& command,
                            bool doesRunWhenDisabled)
        : CommandHelper(std::move(command)),
          m_runsWhenDisabled(doesRunWhenDisabled) {}
    bool RunsWhenDisabled() const override { return m_runsWhenDisabled; }

   private:
    bool m_runsWhenDisabled;
  };

  return std::make_unique<RunsWhenDisabledCommand>(
      std::move(*this).TransferOwnership(), doesRunWhenDisabled);
}

ParallelRaceGroup Command::WithInterrupt(std::function<bool()> condition) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitUntilCommand>(std::move(condition)));
  temp.emplace_back(std::move(*this).TransferOwnership());
  return ParallelRaceGroup(std::move(temp));
}

SequentialCommandGroup Command::BeforeStarting(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).BeforeStarting(
      std::move(toRun), {requirements.begin(), requirements.end()});
}

SequentialCommandGroup Command::BeforeStarting(
    std::function<void()> toRun, wpi::span<Subsystem* const> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  temp.emplace_back(std::move(*this).TransferOwnership());
  return SequentialCommandGroup(std::move(temp));
}

SequentialCommandGroup Command::AndThen(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).AndThen(std::move(toRun),
                                  {requirements.begin(), requirements.end()});
}

SequentialCommandGroup Command::AndThen(
    std::function<void()> toRun, wpi::span<Subsystem* const> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::move(*this).TransferOwnership());
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  return SequentialCommandGroup(std::move(temp));
}

PerpetualCommand Command::Perpetually() && {
  WPI_IGNORE_DEPRECATED
  return PerpetualCommand(std::move(*this).TransferOwnership());
  WPI_UNIGNORE_DEPRECATED
}

EndlessCommand Command::Endlessly() && {
  return EndlessCommand(std::move(*this).TransferOwnership());
}

RepeatCommand Command::Repeat() && {
  return RepeatCommand(std::move(*this).TransferOwnership());
}

ProxyScheduleCommand Command::AsProxy() {
  return ProxyScheduleCommand(this);
}

ConditionalCommand Command::Unless(std::function<bool()> condition) && {
  return ConditionalCommand(std::make_unique<InstantCommand>(),
                            std::move(*this).TransferOwnership(),
                            std::move(condition));
}

void Command::Schedule(bool interruptible) {
  CommandScheduler::GetInstance().Schedule(interruptible, this);
}

void Command::Cancel() {
  CommandScheduler::GetInstance().Cancel(this);
}

bool Command::IsScheduled() const {
  return CommandScheduler::GetInstance().IsScheduled(this);
}

bool Command::HasRequirement(Subsystem* requirement) const {
  bool hasRequirement = false;
  for (auto&& subsystem : GetRequirements()) {
    hasRequirement |= requirement == subsystem;
  }
  return hasRequirement;
}

std::string Command::GetName() const {
  return GetTypeName(*this);
}

bool Command::IsGrouped() const {
  return m_isGrouped;
}

void Command::SetGrouped(bool grouped) {
  m_isGrouped = grouped;
}

namespace frc2 {
bool RequirementsDisjoint(Command* first, Command* second) {
  bool disjoint = true;
  auto&& requirements = second->GetRequirements();
  for (auto&& requirement : first->GetRequirements()) {
    disjoint &= requirements.find(requirement) == requirements.end();
  }
  return disjoint;
}
}  // namespace frc2
