// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <units/time.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that does nothing but ends after a specified match time or
 * condition.  Useful for CommandGroups.
 *
 * This class is provided by the NewCommands VendorDep
 */
class WaitUntilCommand : public CommandHelper<CommandBase, WaitUntilCommand> {
 public:
  /**
   * Creates a new WaitUntilCommand that ends after a given condition becomes
   * true.
   *
   * @param condition the condition to determine when to end
   */
  explicit WaitUntilCommand(std::function<bool()> condition);

  /**
   * Creates a new WaitUntilCommand that ends after a given match time.
   *
   * <p>NOTE: The match timer used for this command is UNOFFICIAL.  Using this
   * command does NOT guarantee that the time at which the action is performed
   * will be judged to be legal by the referees.  When in doubt, add a safety
   * factor or time the action manually.
   *
   * @param time the match time after which to end, in seconds
   */
  explicit WaitUntilCommand(units::second_t time);

  WaitUntilCommand(WaitUntilCommand&& other) = default;

  WaitUntilCommand(const WaitUntilCommand& other) = default;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

 private:
  std::function<bool()> m_condition;
};
}  // namespace frc2
