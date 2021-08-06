// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DriverStationTypes.h>

namespace frc {

/**
 * A wrapper around Driver Station control word.
 */
class DSControlWord {
 public:
  /**
   * DSControlWord constructor.
   *
   * Upon construction, the current Driver Station control word is read and
   * stored internally.
   */
  DSControlWord();

  /**
   * Check if the DS has enabled the robot.
   *
   * @return True if the robot is enabled and the DS is connected
   */
  bool IsEnabled() const;

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  bool IsDisabled() const;

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  bool IsEStopped() const;

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  bool IsAutonomous() const;

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  bool IsAutonomousEnabled() const;

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  bool IsTeleop() const;

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  bool IsTeleopEnabled() const;

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  bool IsTest() const;

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  bool IsDSAttached() const;

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  bool IsFMSAttached() const;

 private:
  HAL_ControlWord m_controlWord;
};

}  // namespace frc
