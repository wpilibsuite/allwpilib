// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <utility>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * Runs one of two commands, depending on the value of the given condition when
 * this command is initialized.  Does not actually schedule the selected command
 * - rather, the command is run through this command; this ensures that the
 * command will behave as expected if used as part of a CommandGroup.  Requires
 * the requirements of both commands, again to ensure proper functioning when
 * used in a CommandGroup.  If this is undesired, consider using
 * ScheduleCommand.
 *
 * <p>As this command contains multiple component commands within it, it is
 * technically a command group; the command instances that are passed to it
 * cannot be added to any other groups, or scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their
 * component commands.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see ScheduleCommand
 */
class ConditionalCommand
    : public CommandHelper<CommandBase, ConditionalCommand> {
 public:
  /**
   * Creates a new ConditionalCommand.
   *
   * @param onTrue    the command to run if the condition is true
   * @param onFalse   the command to run if the condition is false
   * @param condition the condition to determine which command to run
   */
  template <class T1, class T2,
            typename = std::enable_if_t<
                std::is_base_of_v<Command, std::remove_reference_t<T1>>>,
            typename = std::enable_if_t<
                std::is_base_of_v<Command, std::remove_reference_t<T2>>>>
  ConditionalCommand(T1&& onTrue, T2&& onFalse, std::function<bool()> condition)
      : ConditionalCommand(std::make_unique<std::remove_reference_t<T1>>(
                               std::forward<T1>(onTrue)),
                           std::make_unique<std::remove_reference_t<T2>>(
                               std::forward<T2>(onFalse)),
                           condition) {}

  /**
   * Creates a new ConditionalCommand.
   *
   * @param onTrue    the command to run if the condition is true
   * @param onFalse   the command to run if the condition is false
   * @param condition the condition to determine which command to run
   */
  ConditionalCommand(std::unique_ptr<Command>&& onTrue,
                     std::unique_ptr<Command>&& onFalse,
                     std::function<bool()> condition);

  ConditionalCommand(ConditionalCommand&& other) = default;

  // No copy constructors for command groups
  ConditionalCommand(const ConditionalCommand& other) = delete;

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

 private:
  std::unique_ptr<Command> m_onTrue;
  std::unique_ptr<Command> m_onFalse;
  std::function<bool()> m_condition;
  Command* m_selectedCommand{nullptr};
  bool m_runsWhenDisabled = true;
};
}  // namespace frc2
