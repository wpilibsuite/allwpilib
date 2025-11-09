// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/units/time.hpp"

namespace wpi::cmd {
/**
 * A command that does nothing but takes a specified amount of time to finish.
 *
 * This class is provided by the NewCommands VendorDep
 */
class WaitCommand : public CommandHelper<Command, WaitCommand> {
 public:
  /**
   * Creates a new WaitCommand.  This command will do nothing, and end after the
   * specified duration.
   *
   * @param duration the time to wait
   */
  explicit WaitCommand(wpi::units::second_t duration);

  WaitCommand(WaitCommand&& other) = default;

  WaitCommand(const WaitCommand& other) = default;

  void Initialize() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

  bool RunsWhenDisabled() const override;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 protected:
  /// The timer used for waiting.
  wpi::Timer m_timer;

 private:
  wpi::units::second_t m_duration;
};
}  // namespace wpi::cmd
