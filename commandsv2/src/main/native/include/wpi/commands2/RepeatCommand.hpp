// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <concepts>
#include <memory>
#include <utility>

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"

namespace wpi::cmd {
/**
 * A command that runs another command repeatedly, restarting it when it ends,
 * until this command is interrupted. Command instances that are passed to it
 * cannot be added to any other groups, or scheduled individually.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
class RepeatCommand : public CommandHelper<Command, RepeatCommand> {
 public:
  /**
   * Creates a new RepeatCommand. Will run another command repeatedly,
   * restarting it whenever it ends, until this command is interrupted.
   *
   * @param command the command to run repeatedly
   */
  explicit RepeatCommand(std::unique_ptr<Command>&& command);

  /**
   * Creates a new RepeatCommand. Will run another command repeatedly,
   * restarting it whenever it ends, until this command is interrupted.
   *
   * @param command the command to run repeatedly
   */
  template <std::derived_from<Command> T>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  explicit RepeatCommand(T&& command)
      : RepeatCommand(
            std::make_unique<std::decay_t<T>>(std::forward<T>(command))) {}

  RepeatCommand(RepeatCommand&& other) = default;

  // No copy constructors for command groups
  RepeatCommand(const RepeatCommand& other) = delete;

  // Prevent template expansion from emulating copy ctor
  RepeatCommand(RepeatCommand&) = delete;

  void Initialize() override;

  void Execute() override;

  bool IsFinished() override;

  void End(bool interrupted) override;

  bool RunsWhenDisabled() const override;

  Command::InterruptionBehavior GetInterruptionBehavior() const override;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  std::unique_ptr<Command> m_command;
  bool m_ended;
};
}  // namespace wpi::cmd

#ifdef _WIN32
#pragma warning(pop)
#endif
