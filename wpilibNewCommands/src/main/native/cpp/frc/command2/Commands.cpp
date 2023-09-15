// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/Commands.h"

#include "frc/command2/ConditionalCommand.h"
#include "frc/command2/FunctionalCommand.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/ParallelCommandGroup.h"
#include "frc/command2/ParallelDeadlineGroup.h"
#include "frc/command2/ParallelRaceGroup.h"
#include "frc/command2/PrintCommand.h"
#include "frc/command2/ProxyCommand.h"
#include "frc/command2/RunCommand.h"
#include "frc/command2/SequentialCommandGroup.h"
#include "frc/command2/WaitCommand.h"
#include "frc/command2/WaitUntilCommand.h"

using namespace frc;

// Factories

CommandPtr cmd::None() {
  return InstantCommand().ToPtr();
}

CommandPtr cmd::Idle() {
  return Run([] {});
}

CommandPtr cmd::RunOnce(std::function<void()> action,
                        Requirements requirements) {
  return InstantCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::Run(std::function<void()> action, Requirements requirements) {
  return RunCommand(std::move(action), requirements).ToPtr();
}

CommandPtr cmd::StartEnd(std::function<void()> start, std::function<void()> end,
                         Requirements requirements) {
  return FunctionalCommand(
             std::move(start), [] {},
             [end = std::move(end)](bool interrupted) { end(); },
             [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::RunEnd(std::function<void()> run, std::function<void()> end,
                       Requirements requirements) {
  return FunctionalCommand([] {}, std::move(run),
                           [end = std::move(end)](bool interrupted) { end(); },
                           [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::Print(std::string_view msg) {
  return PrintCommand(msg).ToPtr();
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<Command*()> supplier) {
  return ProxyCommand(std::move(supplier)).ToPtr();
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<CommandPtr()> supplier) {
  return ProxyCommand(std::move(supplier)).ToPtr();
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
