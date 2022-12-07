// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <memory>
#include <utility>
#include <vector>

#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A CommandGroup that runs a set of commands in parallel, ending when any one
 * of the commands ends and interrupting all the others.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their
 * component commands.
 *
 * This class is provided by the NewCommands VendorDep
 */
class ParallelRaceGroup
    : public CommandHelper<CommandGroupBase, ParallelRaceGroup> {
 public:
  /**
   * Creates a new ParallelCommandRace.  The given commands will be executed
   * simultaneously, and will "race to the finish" - the first command to finish
   * ends the entire command, with all other commands being interrupted.
   *
   * @param commands the commands to include in this group.
   */
  explicit ParallelRaceGroup(std::vector<std::unique_ptr<Command>>&& commands);

  template <class... Types,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_base_of<Command, std::remove_reference_t<Types>>...>>>
  explicit ParallelRaceGroup(Types&&... commands) {
    AddCommands(std::forward<Types>(commands)...);
  }

  ParallelRaceGroup(ParallelRaceGroup&& other) = default;

  // No copy constructors for command groups
  ParallelRaceGroup(const ParallelRaceGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  ParallelRaceGroup(ParallelRaceGroup&) = delete;

  template <class... Types>
  void AddCommands(Types&&... commands) {
    std::vector<std::unique_ptr<Command>> foo;
    ((void)foo.emplace_back(std::make_unique<std::remove_reference_t<Types>>(
         std::forward<Types>(commands))),
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
  void AddCommands(std::vector<std::unique_ptr<Command>>&& commands) final;

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
