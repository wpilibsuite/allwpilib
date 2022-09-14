// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandPtr.h"

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

CommandPtr CommandPtr::Repeatedly() && {
  m_ptr = std::make_unique<RepeatCommand>(std::move(m_ptr));
  return std::move(*this);
}

CommandPtr CommandPtr::Endlessly() && {
  m_ptr = std::make_unique<EndlessCommand>(std::move(m_ptr));
  return std::move(*this);
}

CommandPtr CommandPtr::AsProxy() && {
  m_ptr = std::make_unique<ProxyScheduleCommand>(std::move(m_ptr));
  return std::move(*this);
}

class RunsWhenDisabledCommand : public WrapperCommand {
 public:
  RunsWhenDisabledCommand(std::unique_ptr<Command>&& command,
                          bool doesRunWhenDisabled)
      : WrapperCommand(std::move(command)),
        m_runsWhenDisabled(doesRunWhenDisabled) {}

  bool RunsWhenDisabled() const override { return m_runsWhenDisabled; }

 private:
  bool m_runsWhenDisabled;
};

CommandPtr CommandPtr::IgnoringDisable(bool doesRunWhenDisabled) && {
  m_ptr = std::make_unique<RunsWhenDisabledCommand>(std::move(m_ptr),
                                                    doesRunWhenDisabled);
  return std::move(*this);
}

using InterruptionBehavior = Command::InterruptionBehavior;
class InterruptBehaviorCommand : public WrapperCommand {
 public:
  InterruptBehaviorCommand(std::unique_ptr<Command>&& command,
                           InterruptionBehavior interruptBehavior)
      : WrapperCommand(std::move(command)),
        m_interruptBehavior(interruptBehavior) {}

  InterruptionBehavior GetInterruptionBehavior() const override {
    return m_interruptBehavior;
  }

 private:
  InterruptionBehavior m_interruptBehavior;
};

CommandPtr CommandPtr::WithInterruptBehavior(
    InterruptionBehavior interruptBehavior) && {
  m_ptr = std::make_unique<InterruptBehaviorCommand>(std::move(m_ptr),
                                                     interruptBehavior);
  return std::move(*this);
}

CommandPtr CommandPtr::AndThen(std::function<void()> toRun,
                               wpi::span<Subsystem* const> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::move(m_ptr));
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  m_ptr = std::make_unique<SequentialCommandGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::AndThen(std::function<void()> toRun,
                               std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).AndThen(std::move(toRun), {requirements.begin(), requirements.end()});
}

CommandPtr CommandPtr::BeforeStarting(
    std::function<void()> toRun, wpi::span<Subsystem* const> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<SequentialCommandGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::BeforeStarting(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).BeforeStarting(std::move(toRun),
                        {requirements.begin(), requirements.end()});
}

CommandPtr CommandPtr::WithTimeout(units::second_t duration) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitCommand>(duration));
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<ParallelRaceGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::Until(std::function<bool()> condition) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitUntilCommand>(std::move(condition)));
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<ParallelRaceGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::Unless(std::function<bool()> condition) && {
  m_ptr = std::make_unique<ConditionalCommand>(
      std::make_unique<InstantCommand>(), std::move(m_ptr),
      std::move(condition));
  return std::move(*this);
}

Command* CommandPtr::operator*() const {
  return m_ptr.get();
}

void CommandPtr::Schedule() const {
  CommandScheduler::GetInstance().Schedule(*this);
}

void CommandPtr::Cancel() const {
  CommandScheduler::GetInstance().Cancel(*this);
}

bool CommandPtr::IsScheduled() const {
  return CommandScheduler::GetInstance().IsScheduled(*this);
}

bool CommandPtr::HasRequirement(Subsystem* requirement) const {
  return m_ptr->HasRequirement(requirement);
}
