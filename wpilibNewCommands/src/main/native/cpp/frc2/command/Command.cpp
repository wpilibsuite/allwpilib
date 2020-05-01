/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/Command.h"

#include "frc2/command/CommandScheduler.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PerpetualCommand.h"
#include "frc2/command/ProxyScheduleCommand.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;

Command::~Command() { CommandScheduler::GetInstance().Cancel(this); }

Command::Command(const Command& rhs) : ErrorBase(rhs) {}

Command& Command::operator=(const Command& rhs) {
  ErrorBase::operator=(rhs);
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
      std::move(toRun),
      wpi::makeArrayRef(requirements.begin(), requirements.end()));
}

SequentialCommandGroup Command::BeforeStarting(
    std::function<void()> toRun, wpi::ArrayRef<Subsystem*> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  temp.emplace_back(std::move(*this).TransferOwnership());
  return SequentialCommandGroup(std::move(temp));
}

SequentialCommandGroup Command::AndThen(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) && {
  return std::move(*this).AndThen(
      std::move(toRun),
      wpi::makeArrayRef(requirements.begin(), requirements.end()));
}

SequentialCommandGroup Command::AndThen(
    std::function<void()> toRun, wpi::ArrayRef<Subsystem*> requirements) && {
  std::vector<std::unique_ptr<Command>> temp;
  temp.emplace_back(std::move(*this).TransferOwnership());
  temp.emplace_back(
      std::make_unique<InstantCommand>(std::move(toRun), requirements));
  return SequentialCommandGroup(std::move(temp));
}

PerpetualCommand Command::Perpetually() && {
  return PerpetualCommand(std::move(*this).TransferOwnership());
}

ProxyScheduleCommand Command::AsProxy() { return ProxyScheduleCommand(this); }

void Command::Schedule(bool interruptible) {
  CommandScheduler::GetInstance().Schedule(interruptible, this);
}

void Command::Cancel() { CommandScheduler::GetInstance().Cancel(this); }

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

std::string Command::GetName() const { return GetTypeName(*this); }

bool Command::IsGrouped() const { return m_isGrouped; }

void Command::SetGrouped(bool grouped) { m_isGrouped = grouped; }

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
