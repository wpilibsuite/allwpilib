// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <units/time.h>

#include "frc2/command/CommandPtr.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/Commands.h"

namespace frc2 {
/**
  CommandRobot is a wrapper over TimedRobot designed to work well with Command
 * based programming.
 *
 * <p>The CommandRobot class is intended to be subclassed by a user when
 creating a
 * robot program.
 */
class CommandRobot : public frc::TimedRobot {
 public:
  void RobotInit() final;

  void RobotPeriodic() final;

  void AutonomousInit() final;

  void AutonomousPeriodic() final;

  void AutonomousExit() final;

  void TeleopInit() final;

  void TeleopPeriodic() final;

  void TeleopExit() final;

  void DisabledInit() final;

  void DisabledPeriodic() final;

  void DisabledExit() final;

 protected:
  /**
   * Constructor for CommandRobot.
   *
   * @param period Period in seconds.
   */
  explicit CommandRobot(units::second_t period = kDefaultPeriod);

  /// The CommandScheduler instance.
  frc2::CommandScheduler& m_scheduler = frc2::CommandScheduler::GetInstance();

  /// The autonomous chooser.
  frc::SendableChooser<frc2::Command*> m_autoChooser;

 private:
  frc2::CommandPtr m_defaultCommand = frc2::cmd::Print(
      "No autos configured. Add Commands to m_autoChooser to fix this.");
  frc2::Command* m_autonomousCommand = nullptr;
};
}  // namespace frc2
