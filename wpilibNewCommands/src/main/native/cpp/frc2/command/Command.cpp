// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Command.h"

#include "frc2/command/CommandHelper.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PerpetualCommand.h"
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

CommandPtr Command::WithTimeout(units::second_t duration) && {
  return std::move(*this).ToPtr().WithTimeout(duration);
}

CommandPtr Command::Until(std::function<bool()> condition) && {
  return std::move(*this).ToPtr().Until(std::move(condition));
}

CommandPtr Command::IgnoringDisable(bool doesRunWhenDisabled) && {
  return std::move(*this).ToPtr().IgnoringDisable(doesRunWhenDisabled);
}

CommandPtr Command::WithInterruptBehavior(
    InterruptionBehavior interruptBehavior) && {
  return std::move(*this).ToPtr().WithInterruptBehavior(interruptBehavior);
}

CommandPtr Command::WithInterrupt(std::function<bool()> condition) && {
  return std::move(*this).ToPtr().Until(std::move(condition));
}

CommandPtr Command::BeforeStarting(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).BeforeStarting(
      std::move(toRun), {requirements.begin(), requirements.end()});
}

CommandPtr Command::BeforeStarting(
    std::function<void()> toRun, std::span<Subsystem* const> requirements) && {
  return std::move(*this).ToPtr().BeforeStarting(std::move(toRun),
                                                 requirements);
}

CommandPtr Command::AndThen(std::function<void()> toRun,
                            std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).AndThen(std::move(toRun),
                                  {requirements.begin(), requirements.end()});
}

CommandPtr Command::AndThen(std::function<void()> toRun,
                            std::span<Subsystem* const> requirements) && {
  return std::move(*this).ToPtr().AndThen(std::move(toRun), requirements);
}

PerpetualCommand Command::Perpetually() && {
  WPI_IGNORE_DEPRECATED
  return PerpetualCommand(std::move(*this).TransferOwnership());
  WPI_UNIGNORE_DEPRECATED
}

CommandPtr Command::Repeatedly() && {
  return std::move(*this).ToPtr().Repeatedly();
}

CommandPtr Command::AsProxy() && {
  return std::move(*this).ToPtr().AsProxy();
}

CommandPtr Command::Unless(std::function<bool()> condition) && {
  return std::move(*this).ToPtr().Unless(std::move(condition));
}

CommandPtr Command::FinallyDo(std::function<void(bool)> end) && {
  return std::move(*this).ToPtr().FinallyDo(std::move(end));
}

CommandPtr Command::HandleInterrupt(std::function<void(void)> handler) && {
  return std::move(*this).ToPtr().HandleInterrupt(std::move(handler));
}

CommandPtr Command::WithName(std::string_view name) && {
  SetName(name);
  return std::move(*this).ToPtr();
}

void Command::Schedule() {
  CommandScheduler::GetInstance().Schedule(this);
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

void Command::SetName(std::string_view name) {}

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
