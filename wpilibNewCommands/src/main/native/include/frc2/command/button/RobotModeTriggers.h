// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc2/command/button/Trigger.h"

namespace frc2 {

/**
 * A class containing static Triggers for running callbacks when robot
 * mode changes.
 */
class RobotModeTriggers {
 public:
  RobotModeTriggers() = delete;

  /**
   * A trigger that is true when the robot is enabled in autonomous
   * mode.
   */
  static Trigger Autonomous;

  /** A trigger that is true when the robot is enabled in teleop mode.
   */
  static Trigger Teleop;

  /**
   * A trigger that is true when the robot is disabled.
   */
  static Trigger Disabled;

  /**
   * A trigger that is true when the robot is enabled in test mode.
   */
  static Trigger Test;
};

Trigger RobotModeTriggers::Autonomous = Trigger{&frc::DriverStation::IsAutonomousEnabled};
Trigger RobotModeTriggers::Teleop = Trigger{&frc::DriverStation::IsTeleopEnabled};
Trigger RobotModeTriggers::Disabled = Trigger{&frc::DriverStation::IsDisabled};
Trigger RobotModeTriggers::Test = Trigger{&frc::DriverStation::IsTestEnabled};

}  // namespace frc2
