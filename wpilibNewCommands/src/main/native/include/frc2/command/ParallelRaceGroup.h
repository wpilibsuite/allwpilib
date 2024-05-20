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
 * A composition that runs a set of commands in parallel, ending when any one of
 * the commands ends and interrupting all the others.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ParallelRaceGroup : public CommandHelper<Command, ParallelRaceGroup> {
 public:
  /**
   * Creates a new ParallelCommandRace. The given commands will be executed
   * simultaneously, and will "race to the finish" - the first command to finish
   * ends the entire command, with all other commands being interrupted.
   *
   * @param commands the commands to include in this composition.
   */
  explicit ParallelRaceGroup(std::vector<std::unique_ptr<Command>>&& commands);

  template <wpi::DecayedDerivedFrom<Command>... Commands>
  explicit ParallelRaceGroup(Commands&&... commands) {
    AddCommands(std::forward<Commands>(commands)...);
  }

  ParallelRaceGroup(ParallelRaceGroup&& other) = default;

  // No copy constructors for command groups
  ParallelRaceGroup(const ParallelRaceGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  ParallelRaceGroup(ParallelRaceGroup&) = delete;

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

  std::vector<std::unique_ptr<Command>> m_commands;
  bool m_runWhenDisabled{true};
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};
  bool m_finished{false};
  bool isRunning = false;
};
}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
