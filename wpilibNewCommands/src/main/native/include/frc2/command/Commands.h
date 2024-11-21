// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <wpi/deprecated.h>

#include "frc2/command/CommandPtr.h"
#include "frc2/command/Requirements.h"
#include "frc2/command/SelectCommand.h"

namespace frc2 {
class Subsystem;

/**
 * Namespace for command factories.
 */
namespace cmd {

/**
 * Constructs a command that does nothing, finishing immediately.
 */
[[nodiscard]]
CommandPtr None();

/**
 * Constructs a command that does nothing until interrupted.
 *
 * @param requirements Subsystems to require
 * @return the command
 */
[[nodiscard]]
CommandPtr Idle(Requirements requirements = {});

// Action Commands

/**
 * Constructs a command that runs an action once and finishes.
 *
 * @param action the action to run
 * @param requirements subsystems the action requires
 */
[[nodiscard]]
CommandPtr RunOnce(std::function<void()> action,
                   Requirements requirements = {});

/**
 * Constructs a command that runs an action every iteration until interrupted.
 *
 * @param action the action to run
 * @param requirements subsystems the action requires
 */
[[nodiscard]]
CommandPtr Run(std::function<void()> action, Requirements requirements = {});

/**
 * Constructs a command that runs an action once and another action when the
 * command is interrupted.
 *
 * @param start the action to run on start
 * @param end the action to run on interrupt
 * @param requirements subsystems the action requires
 */
[[nodiscard]]
CommandPtr StartEnd(std::function<void()> start, std::function<void()> end,
                    Requirements requirements = {});

/**
 * Constructs a command that runs an action every iteration until interrupted,
 * and then runs a second action.
 *
 * @param run the action to run every iteration
 * @param end the action to run on interrupt
 * @param requirements subsystems the action requires
 */
[[nodiscard]]
CommandPtr RunEnd(std::function<void()> run, std::function<void()> end,
                  Requirements requirements = {});

/**
 * Constructs a command that runs an action once, and then runs an action every
 * iteration until interrupted.
 *
 * @param start the action to run on start
 * @param run the action to run every iteration
 * @param requirements subsystems the action requires
 */
[[nodiscard]]
CommandPtr StartRun(std::function<void()> start, std::function<void()> run,
                    Requirements requirements = {});

/**
 * Constructs a command that prints a message and finishes.
 *
 * @param msg the message to print
 */
[[nodiscard]]
CommandPtr Print(std::string_view msg);

// Idling Commands

/**
 * Constructs a command that does nothing, finishing after a specified duration.
 *
 * @param duration after how long the command finishes
 */
[[nodiscard]]
CommandPtr Wait(units::second_t duration);

/**
 * Constructs a command that does nothing, finishing once a condition becomes
 * true.
 *
 * @param condition the condition
 */
[[nodiscard]]
CommandPtr WaitUntil(std::function<bool()> condition);

// Selector Commands

/**
 * Runs one of two commands, based on the boolean selector function.
 *
 * @param onTrue the command to run if the selector function returns true
 * @param onFalse the command to run if the selector function returns false
 * @param selector the selector function
 */
[[nodiscard]]
CommandPtr Either(CommandPtr&& onTrue, CommandPtr&& onFalse,
                  std::function<bool()> selector);

/**
 * Runs one of several commands, based on the selector function.
 *
 * @param selector the selector function
 * @param commands map of commands to select from
 */
template <typename Key, std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr Select(std::function<Key()> selector,
                  std::pair<Key, CommandPtrs>&&... commands) {
  std::vector<std::pair<Key, std::unique_ptr<Command>>> vec;

  ((void)vec.emplace_back(commands.first, std::move(commands.second).Unwrap()),
   ...);

  return SelectCommand(std::move(selector), std::move(vec)).ToPtr();
}

/**
 * Runs the command supplied by the supplier.
 *
 * @param supplier the command supplier
 * @param requirements the set of requirements for this command
 */
[[nodiscard]]
CommandPtr Defer(wpi::unique_function<CommandPtr()> supplier,
                 Requirements requirements);

/**
 * Constructs a command that schedules the command returned from the supplier
 * when initialized, and ends when it is no longer scheduled. The supplier is
 * called when the command is initialized.
 *
 * @param supplier the command supplier
 */
[[nodiscard]]
CommandPtr DeferredProxy(wpi::unique_function<Command*()> supplier);

/**
 * Constructs a command that schedules the command returned from the supplier
 * when initialized, and ends when it is no longer scheduled. The supplier is
 * called when the command is initialized.
 *
 * @param supplier the command supplier
 */
[[nodiscard]]
CommandPtr DeferredProxy(wpi::unique_function<CommandPtr()> supplier);
// Command Groups

namespace impl {

/**
 * Create a vector of commands.
 */
template <std::convertible_to<CommandPtr>... Args>
std::vector<CommandPtr> MakeVector(Args&&... args) {
  std::vector<CommandPtr> data;
  data.reserve(sizeof...(Args));
  (data.emplace_back(std::forward<Args>(args)), ...);
  return data;
}

}  // namespace impl

/**
 * Runs a group of commands in series, one after the other.
 */
[[nodiscard]]
CommandPtr Sequence(std::vector<CommandPtr>&& commands);

/**
 * Runs a group of commands in series, one after the other.
 */
template <std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr Sequence(CommandPtrs&&... commands) {
  return Sequence(impl::MakeVector(std::forward<CommandPtrs>(commands)...));
}

/**
 * Runs a group of commands in series, one after the other. Once the last
 * command ends, the group is restarted.
 */
[[nodiscard]]
CommandPtr RepeatingSequence(std::vector<CommandPtr>&& commands);

/**
 * Runs a group of commands in series, one after the other. Once the last
 * command ends, the group is restarted.
 */
template <std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr RepeatingSequence(CommandPtrs&&... commands) {
  return RepeatingSequence(
      impl::MakeVector(std::forward<CommandPtrs>(commands)...));
}

/**
 * Runs a group of commands at the same time. Ends once all commands in the
 * group finish.
 */
[[nodiscard]]
CommandPtr Parallel(std::vector<CommandPtr>&& commands);

/**
 * Runs a group of commands at the same time. Ends once all commands in the
 * group finish.
 */
template <std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr Parallel(CommandPtrs&&... commands) {
  return Parallel(impl::MakeVector(std::forward<CommandPtrs>(commands)...));
}

/**
 * Runs a group of commands at the same time. Ends once any command in the group
 * finishes, and cancels the others.
 */
[[nodiscard]]
CommandPtr Race(std::vector<CommandPtr>&& commands);

/**
 * Runs a group of commands at the same time. Ends once any command in the group
 * finishes, and cancels the others.
 */
template <std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr Race(CommandPtrs&&... commands) {
  return Race(impl::MakeVector(std::forward<CommandPtrs>(commands)...));
}

/**
 * Runs a group of commands at the same time. Ends once a specific command
 * finishes, and cancels the others.
 */
[[nodiscard]]
CommandPtr Deadline(CommandPtr&& deadline, std::vector<CommandPtr>&& others);

/**
 * Runs a group of commands at the same time. Ends once a specific command
 * finishes, and cancels the others.
 */
template <std::convertible_to<CommandPtr>... CommandPtrs>
[[nodiscard]]
CommandPtr Deadline(CommandPtr&& deadline, CommandPtrs&&... commands) {
  return Deadline(std::move(deadline),
                  impl::MakeVector(std::forward<CommandPtrs>(commands)...));
}

}  // namespace cmd

}  // namespace frc2
