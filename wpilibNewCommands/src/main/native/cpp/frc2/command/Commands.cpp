// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Commands.h"

#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PrintCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;

// Factories

CommandPtr cmd::None() {
  return InstantCommand().ToPtr();
}

CommandPtr cmd::RunOnce(std::function<void()> action,
                        std::initializer_list<Subsystem*> requirements) {
  return InstantCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::RunOnce(std::function<void()> action,
                        std::span<Subsystem* const> requirements) {
  return InstantCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::Run(std::function<void()> action,
                    std::initializer_list<Subsystem*> requirements) {
  return RunCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::Run(std::function<void()> action,
                    std::span<Subsystem* const> requirements) {
  return RunCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::StartEnd(std::function<void()> start, std::function<void()> end,
                         std::initializer_list<Subsystem*> requirements) {
  return FunctionalCommand(
             std::move(start), [] {},
             [end = std::move(end)](bool interrupted) { end(); },
             [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::StartEnd(std::function<void()> start, std::function<void()> end,
                         std::span<Subsystem* const> requirements) {
  return FunctionalCommand(
             std::move(start), [] {},
             [end = std::move(end)](bool interrupted) { end(); },
             [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::RunEnd(std::function<void()> run, std::function<void()> end,
                       std::initializer_list<Subsystem*> requirements) {
  return FunctionalCommand([] {}, std::move(run),
                           [end = std::move(end)](bool interrupted) { end(); },
                           [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::RunEnd(std::function<void()> run, std::function<void()> end,
                       std::span<Subsystem* const> requirements) {
  return FunctionalCommand([] {}, std::move(run),
                           [end = std::move(end)](bool interrupted) { end(); },
                           [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::Print(std::string_view msg) {
  return PrintCommand(msg).ToPtr();
}

CommandPtr cmd::Wait(units::second_t duration) {
  return WaitCommand(duration).ToPtr();
}

CommandPtr cmd::WaitUntil(std::function<bool()> condition) {
  return WaitUntilCommand(condition).ToPtr();
}

CommandPtr cmd::Either(CommandPtr&& onTrue, CommandPtr&& onFalse,
                       std::function<bool()> selector) {
  return ConditionalCommand(std::move(onTrue).Unwrap(),
                            std::move(onFalse).Unwrap(), std::move(selector))
      .ToPtr();
}

CommandPtr cmd::Sequence(std::vector<CommandPtr>&& commands) {
  return SequentialCommandGroup(CommandPtr::UnwrapVector(std::move(commands)))
      .ToPtr();
}

CommandPtr cmd::RepeatingSequence(std::vector<CommandPtr>&& commands) {
  return Sequence(std::move(commands)).Repeatedly();
}

CommandPtr cmd::Parallel(std::vector<CommandPtr>&& commands) {
  return ParallelCommandGroup(CommandPtr::UnwrapVector(std::move(commands)))
      .ToPtr();
}

CommandPtr cmd::Race(std::vector<CommandPtr>&& commands) {
  return ParallelRaceGroup(CommandPtr::UnwrapVector(std::move(commands)))
      .ToPtr();
}

CommandPtr cmd::Deadline(CommandPtr&& deadline,
                         std::vector<CommandPtr>&& others) {
  return ParallelDeadlineGroup(std::move(deadline).Unwrap(),
                               CommandPtr::UnwrapVector(std::move(others)))
      .ToPtr();
}
