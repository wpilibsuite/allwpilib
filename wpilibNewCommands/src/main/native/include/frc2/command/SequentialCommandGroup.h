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
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {

const size_t invalid_index = std::numeric_limits<size_t>::max();

/**
 * A CommandGroups that runs a list of commands in sequence.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their
 * component commands.
 *
 * This class is provided by the NewCommands VendorDep
 */
class SequentialCommandGroup
    : public CommandHelper<CommandGroupBase, SequentialCommandGroup> {
 public:
  /**
   * Creates a new SequentialCommandGroup.  The given commands will be run
   * sequentially, with the CommandGroup finishing when the last command
   * finishes.
   *
   * @param commands the commands to include in this group.
   */
  explicit SequentialCommandGroup(
      std::vector<std::unique_ptr<Command>>&& commands);

  /**
   * Creates a new SequentialCommandGroup.  The given commands will be run
   * sequentially, with the CommandGroup finishing when the last command
   * finishes.
   *
   * @param commands the commands to include in this group.
   */
  template <class... Types,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_base_of<Command, std::remove_reference_t<Types>>...>>>
  explicit SequentialCommandGroup(Types&&... commands) {
    AddCommands(std::forward<Types>(commands)...);
  }

  SequentialCommandGroup(SequentialCommandGroup&& other) = default;

  // No copy constructors for command groups
  SequentialCommandGroup(const SequentialCommandGroup&) = delete;

  // Prevent template expansion from emulating copy ctor
  SequentialCommandGroup(SequentialCommandGroup&) = delete;

  template <class... Types,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_base_of<Command, std::remove_reference_t<Types>>...>>>
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
