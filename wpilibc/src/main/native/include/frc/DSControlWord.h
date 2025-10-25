// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DriverStationTypes.h>

namespace frc {

/**
 * The overall robot mode (not including enabled state).
 */
enum class RobotMode {
  /// Unknown.
  UNKNOWN = HAL_ROBOTMODE_UNKNOWN,
  /// Autonomous.
  AUTONOMOUS = HAL_ROBOTMODE_AUTONOMOUS,
  /// Qualification.
  TELEOPERATED = HAL_ROBOTMODE_TELEOPERATED,
  /// Elimination.
  TEST = HAL_ROBOTMODE_TEST
};

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
  bool IsEnabled() const {
    return HAL_ControlWord_IsEnabled(m_controlWord) &&
           HAL_ControlWord_IsDSAttached(m_controlWord);
  }

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  bool IsDisabled() const { return !IsEnabled(); }

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  bool IsEStopped() const { return HAL_ControlWord_IsEStopped(m_controlWord); }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or
   * disabled.
   *
   * @return robot mode
   */
  RobotMode GetRobotMode() const {
    return static_cast<RobotMode>(HAL_ControlWord_GetRobotMode(m_controlWord));
  }

  /**
   * Gets the current opmode ID.
   *
   * @return opmode
   */
  int64_t GetOpModeId() const {
    return HAL_ControlWord_GetOpModeId(m_controlWord);
  }

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  bool IsAutonomous() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) ==
           HAL_ROBOTMODE_AUTONOMOUS;
  }

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  bool IsAutonomousEnabled() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) ==
               HAL_ROBOTMODE_AUTONOMOUS &&
           IsEnabled();
  }

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  bool IsTeleop() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) ==
           HAL_ROBOTMODE_TELEOPERATED;
  }

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  bool IsTeleopEnabled() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) ==
               HAL_ROBOTMODE_TELEOPERATED &&
           IsEnabled();
  }

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  bool IsTest() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) == HAL_ROBOTMODE_TEST;
  }

  /**
   * Check if the DS is commanding test mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in test mode and
   * enabled.
   */
  bool IsTestEnabled() const {
    return HAL_ControlWord_GetRobotMode(m_controlWord) == HAL_ROBOTMODE_TEST &&
           IsEnabled();
  }

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  bool IsDSAttached() const {
    return HAL_ControlWord_IsDSAttached(m_controlWord);
  }

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  bool IsFMSAttached() const {
    return HAL_ControlWord_IsFMSAttached(m_controlWord);
  }

  /**
   * Get the HAL native value.
   *
   * @return Control word value
   */
  HAL_ControlWord GetControlWord() const { return m_controlWord; }

 private:
  HAL_ControlWord m_controlWord;
};

}  // namespace frc
