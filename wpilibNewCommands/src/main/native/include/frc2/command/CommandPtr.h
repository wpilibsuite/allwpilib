// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>

#include "frc2/command/Command.h"

namespace frc2 {
class CommandPtr final {
 public:
  explicit CommandPtr(std::unique_ptr<Command>&& command)
      : m_ptr(std::move(command)) {}

  template <class T, typename = std::enable_if_t<std::is_base_of_v<
                         Command, std::remove_reference_t<T>>>>
  explicit CommandPtr(T&& command)
      : CommandPtr(std::make_unique<std::remove_reference_t<T>>(
            std::forward<T>(command))) {}

  CommandPtr(CommandPtr&&) = default;
  CommandPtr& operator=(CommandPtr&&) = default;

  /**
   * Decorates this command to run repeatedly, restarting it when it ends, until
   * this command is interrupted. The decorated command can still be canceled.
   *
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr Repeatedly() &&;

  /**
   * Decorates this command to run endlessly, ignoring its ordinary end
   * conditions. The decorated command can still be interrupted or canceled.
   *
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr Endlessly() &&;

  /**
   * Decorates this command to run "by proxy" by wrapping it in a
   * ProxyScheduleCommand. This is useful for "forking off" from command groups
   * when the user does not wish to extend the command's requirements to the
   * entire command group. 
   *
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AsProxy() &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param doesRunWhenDisabled true to run when disabled.
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr IgnoringDisable(bool doesRunWhenDisabled) &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param interruptBehavior true to run when disabled.
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr WithInterruptBehavior(
      Command::InterruptionBehavior interruptBehavior) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AndThen(
      std::function<void()> toRun,
      wpi::span<Subsystem* const> requirements = {}) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AndThen(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr BeforeStarting(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr BeforeStarting(
      std::function<void()> toRun,
      wpi::span<Subsystem* const> requirements = {}) &&;

  /**
   * Decorates this command with a timeout.  If the specified timeout is
   * exceeded before the command finishes normally, the command will be
   * interrupted and un-scheduled.  Note that the timeout only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param duration the timeout duration
   * @return the command with the timeout added
   */
  [[nodiscard]] CommandPtr WithTimeout(units::second_t duration) &&;

  /**
   * Decorates this command with an interrupt condition.  If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled. Note that this only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  [[nodiscard]] CommandPtr Until(std::function<bool()> condition) &&;

  /**
   * Decorates this command to only run if this condition is not met. If the
   * command is already running and the condition changes to true, the command
   * will not stop running. The requirements of this command will be kept for
   * the new conditonal command.
   *
   * @param condition the condition that will prevent the command from running
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr Unless(std::function<bool()> condition) &&;

  /**
   * Get a raw pointer to the held command.
   */
  Command* operator*() const;

  /**
   * Schedules this command.
   */
  void Schedule() const;

  /**
   * Cancels this command. Will call End(true). Commands will be canceled
   * regardless of interruption behavior.
   */
  void Cancel() const;

  /**
   * Whether or not the command is currently scheduled. Note that this does not
   * detect whether the command is being run by a CommandGroup, only whether it
   * is directly being run by the scheduler.
   *
   * @return Whether the command is scheduled.
   */
  bool IsScheduled() const;

  /**
   * Whether the command requires a given subsystem.  Named "HasRequirement"
   * rather than "requires" to avoid confusion with Command::Requires(Subsystem)
   * -- this may be able to be changed in a few years.
   *
   * @param requirement the subsystem to inquire about
   * @return whether the subsystem is required
   */
  bool HasRequirement(Subsystem* requirement) const;

 private:
  std::unique_ptr<Command> m_ptr;
};

}  // namespace frc2
