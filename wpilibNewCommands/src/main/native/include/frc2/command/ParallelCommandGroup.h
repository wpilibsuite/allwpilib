// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <wpi/DecayedDerivedFrom.h>

#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command composition that runs a set of commands in parallel, ending when
 * the last command ends.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ParallelCommandGroup
    : public CommandHelper<Command, ParallelCommandGroup> {
 public:
  /**
   * Creates a new ParallelCommandGroup. The given commands will be executed
   * simultaneously. The command group will finish when the last command
   * finishes. If the composition is interrupted, only the commands that are
   * still running will be interrupted.
   *
   * @param commands the commands to include in this composition.
   */
  explicit ParallelCommandGroup(
      std::vector<std::unique_ptr<Command>>&& commands);

  /**
   * Creates a new ParallelCommandGroup. The given commands will be executed
   * simultaneously. The command group will finish when the last command
   * finishes. If the composition is interrupted, only the commands that are
   * still running will be interrupted.
   *
   * @param commands the commands to include in this composition.
   */
  template <wpi::DecayedDerivedFrom<Command>... Commands>
  explicit ParallelCommandGroup(Commands&&... commands) {
    AddCommands(std::forward<Commands>(commands)...);
  }

  ParallelCommandGroup(ParallelCommandGroup&& other) = default;

  // No copy constructors for command groups
  ParallelCommandGroup(const ParallelCommandGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  ParallelCommandGroup(ParallelCommandGroup&) = delete;

  /**
   * Adds the given commands to the group.
   *
   * @param commands Commands to add to the group.
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

 private:
  void AddCommands(std::vector<std::unique_ptr<Command>>&& commands);

  std::vector<std::pair<std::unique_ptr<Command>, bool>> m_commands;
  bool m_runWhenDisabled{true};
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};
  bool isRunning = false;
};
}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
