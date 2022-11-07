// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Timer.h>
#include <units/time.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that does nothing but takes a specified amount of time to finish.
 * Useful for CommandGroups.  Can also be subclassed to make a command with an
 * internal timer.
 *
 * This class is provided by the NewCommands VendorDep
 */
class WaitCommand : public CommandHelper<CommandBase, WaitCommand> {
 public:
  /**
   * Creates a new WaitCommand.  This command will do nothing, and end after the
   * specified duration.
   *
   * @param duration the time to wait
   */
  explicit WaitCommand(units::second_t duration);

  WaitCommand(WaitCommand&& other) = default;

  WaitCommand(const WaitCommand& other) = default;

  void Initialize() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

 protected:
  frc::Timer m_timer;

 private:
  units::second_t m_duration;
};
}  // namespace frc2
