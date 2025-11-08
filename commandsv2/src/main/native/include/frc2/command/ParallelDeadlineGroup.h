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
 * A command composition that runs a set of commands in parallel, ending only
 * when a specific command (the "deadline") ends, interrupting all other
 * commands that are still running at that point.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ParallelDeadlineGroup
    : public CommandHelper<Command, ParallelDeadlineGroup> {
 public:
  /**
   * Creates a new ParallelDeadlineGroup. The given commands (including the
   * deadline) will be executed simultaneously. The composition will finish when
   * the deadline finishes, interrupting all other still-running commands. If
   * the composition is interrupted, only the commands still running will be
   * interrupted.
   *
   * @param deadline the command that determines when the composition ends
   * @param commands the commands to be executed
   */
  ParallelDeadlineGroup(std::unique_ptr<Command>&& deadline,
                        std::vector<std::unique_ptr<Command>>&& commands);
  /**
   * Creates a new ParallelDeadlineGroup. The given commands (including the
   * deadline) will be executed simultaneously. The composition will finish when
   * the deadline finishes, interrupting all other still-running commands. If
   * the composition is interrupted, only the commands still running will be
   * interrupted.
   *
   * @param deadline the command that determines when the composition ends
   * @param commands the commands to be executed
   */
  template <wpi::DecayedDerivedFrom<Command> T,
            wpi::DecayedDerivedFrom<Command>... Commands>
  explicit ParallelDeadlineGroup(T&& deadline, Commands&&... commands) {
    SetDeadline(std::make_unique<std::decay_t<T>>(std::forward<T>(deadline)));
    AddCommands(std::forward<Commands>(commands)...);
  }

  ParallelDeadlineGroup(ParallelDeadlineGroup&& other) = default;

  // No copy constructors for command groups
  ParallelDeadlineGroup(const ParallelDeadlineGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  ParallelDeadlineGroup(ParallelDeadlineGroup&) = delete;

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

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void AddCommands(std::vector<std::unique_ptr<Command>>&& commands);

  void SetDeadline(std::unique_ptr<Command>&& deadline);

  std::vector<std::pair<std::unique_ptr<Command>, bool>> m_commands;
  Command* m_deadline;
  bool m_runWhenDisabled{true};
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};
  bool m_finished{true};
};
}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
