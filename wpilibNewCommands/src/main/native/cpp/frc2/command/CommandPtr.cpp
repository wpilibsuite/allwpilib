// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandPtr.h"

#include <frc/Errors.h>

#include "frc2/command/CommandScheduler.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PrintCommand.h"
#include "frc2/command/ProxyCommand.h"
#include "frc2/command/RepeatCommand.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/WrapperCommand.h"

using namespace frc2;

void CommandPtr::AssertValid() const {
  if (!m_ptr) {
    throw FRC_MakeError(frc::err::CommandIllegalUse,
                        "Moved-from CommandPtr object used!");
  }
}

CommandPtr CommandPtr::Repeatedly() && {
  AssertValid();
  m_ptr = std::make_unique<RepeatCommand>(std::move(m_ptr));
  return std::move(*this);
}

CommandPtr CommandPtr::AsProxy() && {
  AssertValid();
  m_ptr = std::make_unique<ProxyCommand>(std::move(m_ptr));
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
  AssertValid();
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
  AssertValid();
  m_ptr = std::make_unique<InterruptBehaviorCommand>(std::move(m_ptr),
                                                     interruptBehavior);
  return std::move(*this);
}

CommandPtr CommandPtr::AndThen(std::function<void()> toRun,
                               std::span<Subsystem* const> requirements) && {
  AssertValid();
  return std::move(*this).AndThen(CommandPtr(
      std::make_unique<InstantCommand>(std::move(toRun), requirements)));
}

CommandPtr CommandPtr::AndThen(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  AssertValid();
  return std::move(*this).AndThen(CommandPtr(
      std::make_unique<InstantCommand>(std::move(toRun), requirements)));
}

CommandPtr CommandPtr::AndThen(CommandPtr&& next) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::move(m_ptr));
  temp.emplace_back(std::move(next).Unwrap());
  m_ptr = std::make_unique<SequentialCommandGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::BeforeStarting(
    std::function<void()> toRun, std::span<Subsystem* const> requirements) && {
  AssertValid();
  return std::move(*this).BeforeStarting(CommandPtr(
      std::make_unique<InstantCommand>(std::move(toRun), requirements)));
}

CommandPtr CommandPtr::BeforeStarting(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  AssertValid();
  return std::move(*this).BeforeStarting(CommandPtr(
      std::make_unique<InstantCommand>(std::move(toRun), requirements)));
}

CommandPtr CommandPtr::BeforeStarting(CommandPtr&& before) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::move(before).Unwrap());
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<SequentialCommandGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::WithTimeout(units::second_t duration) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitCommand>(duration));
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<ParallelRaceGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::Until(std::function<bool()> condition) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::make_unique<WaitUntilCommand>(std::move(condition)));
  temp.emplace_back(std::move(m_ptr));
  m_ptr = std::make_unique<ParallelRaceGroup>(std::move(temp));
  return std::move(*this);
}

CommandPtr CommandPtr::Unless(std::function<bool()> condition) && {
  AssertValid();
  m_ptr = std::make_unique<ConditionalCommand>(
      std::make_unique<InstantCommand>(), std::move(m_ptr),
      std::move(condition));
  return std::move(*this);
}

CommandPtr CommandPtr::DeadlineWith(CommandPtr&& parallel) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> vec;
  vec.emplace_back(std::move(parallel).Unwrap());
  m_ptr =
      std::make_unique<ParallelDeadlineGroup>(std::move(m_ptr), std::move(vec));
  return std::move(*this);
}

CommandPtr CommandPtr::AlongWith(CommandPtr&& parallel) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> vec;
  vec.emplace_back(std::move(m_ptr));
  vec.emplace_back(std::move(parallel).Unwrap());
  m_ptr = std::make_unique<ParallelCommandGroup>(std::move(vec));
  return std::move(*this);
}

CommandPtr CommandPtr::RaceWith(CommandPtr&& parallel) && {
  AssertValid();
  std::vector<std::unique_ptr<Command>> vec;
  vec.emplace_back(std::move(m_ptr));
  vec.emplace_back(std::move(parallel).Unwrap());
  m_ptr = std::make_unique<ParallelRaceGroup>(std::move(vec));
  return std::move(*this);
}

namespace {
class FinallyCommand : public WrapperCommand {
 public:
  FinallyCommand(std::unique_ptr<Command>&& command,
                 std::function<void(bool)> end)
      : WrapperCommand(std::move(command)), m_end(std::move(end)) {}

  void End(bool interrupted) override {
    WrapperCommand::End(interrupted);
    m_end(interrupted);
  }

 private:
  std::function<void(bool)> m_end;
};
}  // namespace

CommandPtr CommandPtr::FinallyDo(std::function<void(bool)> end) && {
  AssertValid();
  m_ptr = std::make_unique<FinallyCommand>(std::move(m_ptr), std::move(end));
  return std::move(*this);
}

CommandPtr CommandPtr::HandleInterrupt(std::function<void(void)> handler) && {
  AssertValid();
  return std::move(*this).FinallyDo(
      [handler = std::move(handler)](bool interrupted) {
        if (interrupted) {
          handler();
        }
      });
}

CommandPtr CommandPtr::WithName(std::string_view name) && {
  AssertValid();
  m_ptr->SetName(name);
  return std::move(*this);
}

CommandBase* CommandPtr::get() const {
  AssertValid();
  return m_ptr.get();
}

std::unique_ptr<CommandBase> CommandPtr::Unwrap() && {
  AssertValid();
  return std::move(m_ptr);
}

void CommandPtr::Schedule() const {
  AssertValid();
  CommandScheduler::GetInstance().Schedule(*this);
}

void CommandPtr::Cancel() const {
  AssertValid();
  CommandScheduler::GetInstance().Cancel(*this);
}

bool CommandPtr::IsScheduled() const {
  AssertValid();
  return CommandScheduler::GetInstance().IsScheduled(*this);
}

bool CommandPtr::HasRequirement(Subsystem* requirement) const {
  AssertValid();
  return m_ptr->HasRequirement(requirement);
}

CommandPtr::operator bool() const {
  return m_ptr.operator bool();
}

std::vector<std::unique_ptr<Command>> CommandPtr::UnwrapVector(
    std::vector<CommandPtr>&& vec) {
  std::vector<std::unique_ptr<Command>> ptrs;
  for (auto&& ptr : vec) {
    ptrs.emplace_back(std::move(ptr).Unwrap());
  }
  return ptrs;
}
