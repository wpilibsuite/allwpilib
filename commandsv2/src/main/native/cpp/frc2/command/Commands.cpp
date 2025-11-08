// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/Commands.hpp"

#include <utility>
#include <vector>

#include "wpi/commands2/ConditionalCommand.hpp"
#include "wpi/commands2/DeferredCommand.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/InstantCommand.hpp"
#include "wpi/commands2/ParallelCommandGroup.hpp"
#include "wpi/commands2/ParallelDeadlineGroup.hpp"
#include "wpi/commands2/ParallelRaceGroup.hpp"
#include "wpi/commands2/PrintCommand.hpp"
#include "wpi/commands2/ProxyCommand.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/commands2/SequentialCommandGroup.hpp"
#include "wpi/commands2/WaitCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/util/FunctionExtras.hpp"
#include "wpi/util/deprecated.hpp"

using namespace wpi::cmd;

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

CommandPtr cmd::DeferredProxy(wpi::util::unique_function<Command*()> supplier) {
  return Defer(
      [supplier = std::move(supplier)]() mutable {
        // There is no non-owning version of AsProxy(), so use the non-owning
        // ProxyCommand constructor instead.
        return ProxyCommand{supplier()}.ToPtr();
      },
      {});
}

CommandPtr cmd::DeferredProxy(wpi::util::unique_function<CommandPtr()> supplier) {
  return Defer([supplier = std::move(
                    supplier)]() mutable { return supplier().AsProxy(); },
               {});
}

CommandPtr cmd::Wait(wpi::units::second_t duration) {
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

CommandPtr cmd::Defer(wpi::util::unique_function<CommandPtr()> supplier,
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
