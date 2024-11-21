// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Commands.h"

#include <utility>
#include <vector>

#include <wpi/FunctionExtras.h>
#include <wpi/deprecated.h>

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
  return InstantCommand().ToPtr();
}

CommandPtr cmd::Idle(Requirements requirements) {
  return Run([] {}, requirements);
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

CommandPtr cmd::StartRun(std::function<void()> start, std::function<void()> run,
                         Requirements requirements) {
  return FunctionalCommand(
             std::move(start), std::move(run), [](bool interrupted) {},
             [] { return false; }, requirements)
      .ToPtr();
}

CommandPtr cmd::Print(std::string_view msg) {
  return PrintCommand(msg).ToPtr();
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<Command*()> supplier) {
  return Defer(
      [supplier = std::move(supplier)]() mutable {
        // There is no non-owning version of AsProxy(), so use the non-owning
        // ProxyCommand constructor instead.
        return ProxyCommand{supplier()}.ToPtr();
      },
      {});
}

CommandPtr cmd::DeferredProxy(wpi::unique_function<CommandPtr()> supplier) {
  return Defer([supplier = std::move(
                    supplier)]() mutable { return supplier().AsProxy(); },
               {});
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

CommandPtr cmd::Defer(wpi::unique_function<CommandPtr()> supplier,
                      Requirements requirements) {
  return DeferredCommand(std::move(supplier), requirements).ToPtr();
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
