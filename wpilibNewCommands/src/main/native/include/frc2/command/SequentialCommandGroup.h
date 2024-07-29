// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <wpi/DecayedDerivedFrom.h>

#include "frc2/command/CommandHelper.h"

namespace frc2 {

const size_t invalid_index = std::numeric_limits<size_t>::max();

/**
 * A command composition that runs a list of commands in sequence.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * This class is provided by the NewCommands VendorDep
 */
class SequentialCommandGroup
    : public CommandHelper<Command, SequentialCommandGroup> {
 public:
  /**
   * Creates a new SequentialCommandGroup. The given commands will be run
   * sequentially, with the composition finishing when the last command
   * finishes.
   *
   * @param commands the commands to include in this composition.
   */
  explicit SequentialCommandGroup(
      std::vector<std::unique_ptr<Command>>&& commands);

  /**
   * Creates a new SequentialCommandGroup. The given commands will be run
   * sequentially, with the composition finishing when the last command
   * finishes.
   *
   * @param commands the commands to include in this composition.
   */
  template <wpi::DecayedDerivedFrom<Command>... Commands>
  explicit SequentialCommandGroup(Commands&&... commands) {
    AddCommands(std::forward<Commands>(commands)...);
  }

  SequentialCommandGroup(SequentialCommandGroup&& other) = default;

  // No copy constructors for command groups
  SequentialCommandGroup(const SequentialCommandGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  SequentialCommandGroup(SequentialCommandGroup&) = delete;

  /**
   * Adds the given commands to the group.
   *
   * @param commands Commands to add, in order of execution.
   */
  template <wpi::DecayedDerivedFrom<Command>... Commands>
  void AddCommands(Commands&&... commands) {
    std::vector<std::unique_ptr<Command>> foo;
    ((void)foo.emplace_back(std::make_unique<std::decay_t<Commands>>(
         std::forward<Commands>(commands))),
     ...);
    AddCommands(std::move(foo));
  }

  void Initialize() final;

  void Execute() final;

  void End(bool interrupted) final;

  bool IsFinished() final;

  bool RunsWhenDisabled() const override;

  Command::InterruptionBehavior GetInterruptionBehavior() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void AddCommands(std::vector<std::unique_ptr<Command>>&& commands);

  wpi::SmallVector<std::unique_ptr<Command>, 4> m_commands;
  size_t m_currentCommandIndex{invalid_index};
  bool m_runWhenDisabled{true};
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};
};
}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
