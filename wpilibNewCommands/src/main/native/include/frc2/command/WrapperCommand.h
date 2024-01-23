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

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A class used internally to wrap commands while overriding a specific method;
 * all other methods will call through to the wrapped command.
 *
 * <p>Wrapped commands may only be used through the wrapper, trying to directly
 * schedule them or add them to a group will throw an exception.
 */
class WrapperCommand : public CommandHelper<Command, WrapperCommand> {
 public:
  /**
   * Wrap a command.
   *
   * @param command the command being wrapped. Trying to directly schedule this
   * command or add it to a group will throw an exception.
   */
  explicit WrapperCommand(std::unique_ptr<Command>&& command);

  /**
   * Wrap a command.
   *
   * @param command the command being wrapped. Trying to directly schedule this
   * command or add it to a group will throw an exception.
   */
  template <std::derived_from<Command> T>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  explicit WrapperCommand(T&& command)
      : WrapperCommand(
            std::make_unique<std::decay_t<T>>(std::forward<T>(command))) {}

  WrapperCommand(WrapperCommand&& other) = default;

  // No copy constructors for command groups
  WrapperCommand(const WrapperCommand& other) = delete;

  // Prevent template expansion from emulating copy ctor
  WrapperCommand(WrapperCommand&) = delete;

  void Initialize() override;

  void Execute() override;

  bool IsFinished() override;

  void End(bool interrupted) override;

  bool RunsWhenDisabled() const override;

  InterruptionBehavior GetInterruptionBehavior() const override;

  wpi::SmallSet<Subsystem*, 4> GetRequirements() const override;

 protected:
  /// Command being wrapped.
  std::unique_ptr<Command> m_command;
};
}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
