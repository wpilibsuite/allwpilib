// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Commands.h"

#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/DeferredCommand.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PrintCommand.h"
#include "frc2/command/ProxyCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;

// Factories

CommandPtr cmd::None() {
  return InstantCommand();
}

CommandPtr cmd::Idle(Requirements requirements) {
  return Run([] {}, requirements);
}

CommandPtr cmd::RunOnce(std::function<void()> action,
                        Requirements requirements) {
  return InstantCommand(std::move(action), requirements);
}

CommandPtr cmd::Run(std::function<void()> action, Requirements requirements) {
  return RunCommand(std::move(action), requirements);
}

CommandPtr cmd::StartEnd(std::function<void()> start, std::function<void()> end,
                         Requirements requirements) {
  return FunctionalCommand(
             std::move(start), [] {},
             [end = std::move(end)](bool interrupted) { end(); },
             [] { return false; }, requirements)
      ;
}

CommandPtr cmd::RunEnd(std::function<void()> run, std::function<void()> end,
                       Requirements requirements) {
  return FunctionalCommand([] {}, std::move(run),
                           [end = std::move(end)](bool interrupted) { end(); },
                           [] { return false; }, requirements)
      ;
}

CommandPtr cmd::Print(std::string_view msg) {
  return PrintCommand(msg);
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<Command*()> supplier) {
  return ProxyCommand(std::move(supplier));
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<CommandPtr()> supplier) {
  return ProxyCommand(std::move(supplier));
}

CommandPtr cmd::Wait(units::second_t duration) {
  return WaitCommand(duration);
}

CommandPtr cmd::WaitUntil(std::function<bool()> condition) {
  return WaitUntilCommand(condition);
}

CommandPtr cmd::Either(CommandPtr&& onTrue, CommandPtr&& onFalse,
                       std::function<bool()> selector) {
  return ConditionalCommand(std::move(onTrue).Unwrap(),
                            std::move(onFalse).Unwrap(), std::move(selector))
      ;
}

CommandPtr cmd::Defer(wpi::unique_function<CommandPtr()> supplier,
                      Requirements requirements) {
  return DeferredCommand(std::move(supplier), requirements);
}

CommandPtr cmd::Sequence(std::vector<CommandPtr>&& commands) {
  return SequentialCommandGroup(CommandPtr::UnwrapVector(std::move(commands)))
      ;
}

CommandPtr cmd::RepeatingSequence(std::vector<CommandPtr>&& commands) {
  return Sequence(std::move(commands)).Repeatedly();
}

CommandPtr cmd::Parallel(std::vector<CommandPtr>&& commands) {
  return ParallelCommandGroup(CommandPtr::UnwrapVector(std::move(commands)))
      ;
}

CommandPtr cmd::Race(std::vector<CommandPtr>&& commands) {
  return ParallelRaceGroup(CommandPtr::UnwrapVector(std::move(commands)))
      ;
}

CommandPtr cmd::Deadline(CommandPtr&& deadline,
                         std::vector<CommandPtr>&& others) {
  return ParallelDeadlineGroup(std::move(deadline).Unwrap(),
                               CommandPtr::UnwrapVector(std::move(others)))
      ;
}
