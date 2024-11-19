// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "frc2/command/Command.h"
#include "frc2/command/Requirements.h"

namespace frc2 {
/**
 * A wrapper around std::unique_ptr<Command> so commands have move-only
 * semantics. Commands should only be stored and passed around when held in a
 * CommandPtr instance. For more info, see
 * https://github.com/wpilibsuite/allwpilib/issues/4303.
 *
 * Various classes in the command-based library accept a
 * std::unique_ptr<Command>, use CommandPtr::Unwrap to convert.
 * CommandPtr::UnwrapVector does the same for vectors.
 */
class CommandPtr final {
 public:
  explicit CommandPtr(std::unique_ptr<Command>&& command);

  template <std::derived_from<Command> T>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  explicit CommandPtr(T&& command)
      : CommandPtr(
            std::make_unique<std::decay_t<T>>(std::forward<T>(command))) {}

  CommandPtr(CommandPtr&&);
  CommandPtr& operator=(CommandPtr&&) = default;

  explicit CommandPtr(std::nullptr_t) = delete;

  /**
   * Decorates this command to run repeatedly, restarting it when it ends, until
   * this command is interrupted. The decorated command can still be canceled.
   *
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr Repeatedly() &&;

  /**
   * Decorates this command to run "by proxy" by wrapping it in a ProxyCommand.
   * Use this for "forking off" from command compositions when the user does not
   * wish to extend the command's requirements to the entire command
   * composition. ProxyCommand has unique implications and semantics, see <a
   * href="https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands">the
   * WPILib docs</a> for a full explanation.
   *
   * @return the decorated command
   * @see ProxyCommand
   */
  [[nodiscard]]
  CommandPtr AsProxy() &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param doesRunWhenDisabled true to run when disabled
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr IgnoringDisable(bool doesRunWhenDisabled) &&;

  /**
   * Decorates this command to have a different interrupt behavior.
   *
   * @param interruptBehavior the desired interrupt behavior
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr WithInterruptBehavior(
      Command::InterruptionBehavior interruptBehavior) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AndThen(std::function<void()> toRun,
                     Requirements requirements = {}) &&;

  /**
   * Decorates this command with a set of commands to run after it in sequence.
   * Often more convenient/less-verbose than constructing a new {@link
   * SequentialCommandGroup} explicitly.
   *
   * @param next the commands to run next
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AndThen(CommandPtr&& next) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr BeforeStarting(std::function<void()> toRun,
                            Requirements requirements = {}) &&;

  /**
   * Decorates this command with another command to run before this command
   * starts.
   *
   * @param before the command to run before this one
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr BeforeStarting(CommandPtr&& before) &&;

  /**
   * Decorates this command with a timeout. If the specified timeout is
   * exceeded before the command finishes normally, the command will be
   * interrupted and un-scheduled.
   *
   * @param duration the timeout duration
   * @return the command with the timeout added
   */
  [[nodiscard]]
  CommandPtr WithTimeout(units::second_t duration) &&;

  /**
   * Decorates this command with an interrupt condition. If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  [[nodiscard]]
  CommandPtr Until(std::function<bool()> condition) &&;

  /**
   * Decorates this command with a run condition. If the specified condition
   * becomes false before the command finishes normally, the command will be
   * interrupted and un-scheduled.
   *
   * @param condition the run condition
   * @return the command with the run condition added
   */
  [[nodiscard]]
  CommandPtr OnlyWhile(std::function<bool()> condition) &&;

  /**
   * Decorates this command to only run if this condition is not met. If the
   * command is already running and the condition changes to true, the command
   * will not stop running. The requirements of this command will be kept for
   * the new conditional command.
   *
   * @param condition the condition that will prevent the command from running
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr Unless(std::function<bool()> condition) &&;

  /**
   * Decorates this command to only run if this condition is met. If the command
   * is already running and the condition changes to false, the command will not
   * stop running. The requirements of this command will be kept for the new
   * conditional command.
   *
   * @param condition the condition that will allow the command to run
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr OnlyIf(std::function<bool()> condition) &&;

  /**
   * Creates a new command that runs this command and the deadline in parallel,
   * finishing (and interrupting this command) when the deadline finishes.
   *
   * @param deadline the deadline of the command group
   * @return the decorated command
   * @see DeadlineFor
   */
  CommandPtr WithDeadline(CommandPtr&& deadline) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the calling command ends and interrupting all the others. Often more
   * convenient/less-verbose than constructing a new {@link
   * ParallelDeadlineGroup} explicitly.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  [[nodiscard]] [[deprecated("Replace with DeadlineFor")]]
  CommandPtr DeadlineWith(CommandPtr&& parallel) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the calling command ends and interrupting all the others. Often more
   * convenient/less-verbose than constructing a new {@link
   * ParallelDeadlineGroup} explicitly.
   *
   * @param parallel the commands to run in parallel. Note the parallel commands
   * will be interupted when the deadline command ends
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr DeadlineFor(CommandPtr&& parallel) &&;
  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the last command ends. Often more convenient/less-verbose than
   * constructing a new {@link ParallelCommandGroup} explicitly.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AlongWith(CommandPtr&& parallel) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the first command ends. Often more convenient/less-verbose than
   * constructing a new {@link ParallelRaceGroup} explicitly.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr RaceWith(CommandPtr&& parallel) &&;

  /**
   * Decorates this command with a lambda to call on interrupt or end, following
   * the command's inherent Command::End(bool) method.
   *
   * @param end a lambda accepting a boolean parameter specifying whether the
   * command was interrupted
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr FinallyDo(std::function<void(bool)> end) &&;

  /**
   * Decorates this command with a lambda to call on interrupt or end, following
   * the command's inherent Command::End(bool) method. The provided lambda will
   * run identically in both interrupt and end cases.
   *
   * @param end a lambda to run when the command ends, whether or not it was
   * interrupted.
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr FinallyDo(std::function<void()> end) &&;

  /**
   * Decorates this command with a lambda to call on interrupt, following the
   * command's inherent Command::End(bool) method.
   *
   * @param handler a lambda to run when the command is interrupted
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr HandleInterrupt(std::function<void()> handler) &&;

  /**
   * Decorates this Command with a name. Is an inline function for
   * Command::SetName(std::string_view);
   *
   * @param name name
   * @return the decorated Command
   */
  [[nodiscard]]
  CommandPtr WithName(std::string_view name) &&;

  /**
   * Get a raw pointer to the held command.
   */
  Command* get() const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  Command* get() && = delete;

  /**
   * Convert to the underlying unique_ptr.
   */
  std::unique_ptr<Command> Unwrap() &&;

  /**
   * Schedules this command.
   */
  void Schedule() const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  void Schedule() && = delete;

  /**
   * Cancels this command. Will call End(true). Commands will be canceled
   * regardless of interruption behavior.
   */
  void Cancel() const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  void Cancel() && = delete;

  /**
   * Whether or not the command is currently scheduled. Note that this does not
   * detect whether the command is in a composition, only whether it is directly
   * being run by the scheduler.
   *
   * @return Whether the command is scheduled.
   */
  bool IsScheduled() const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  void IsScheduled() && = delete;

  /**
   * Whether the command requires a given subsystem.  Named "HasRequirement"
   * rather than "requires" to avoid confusion with Command::Requires(Subsystem)
   * -- this may be able to be changed in a few years.
   *
   * @param requirement the subsystem to inquire about
   * @return whether the subsystem is required
   */
  bool HasRequirement(Subsystem* requirement) const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  void HasRequirement(Subsystem* requirement) && = delete;

  /**
   * Check if this CommandPtr object is valid and wasn't moved-from.
   */
  explicit operator bool() const&;

  // Prevent calls on a temporary, as the returned pointer would be invalid
  explicit operator bool() && = delete;

  /**
   * Convert a vector of CommandPtr objects to their underlying unique_ptrs.
   */
  static std::vector<std::unique_ptr<Command>> UnwrapVector(
      std::vector<CommandPtr>&& vec);

 private:
  std::unique_ptr<Command> m_ptr;
  std::string m_moveOutSite{""};
  void AssertValid() const;
};

}  // namespace frc2
