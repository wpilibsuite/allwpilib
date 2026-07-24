// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "wpi/hal/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/util/Color.hpp"

namespace wpi {

using wpi::hal::RobotMode;

/**
 * Provides access to robot state information from the Driver Station.
 */
class RobotState final {
 public:
  RobotState() = delete;

  /**
   * Check if the DS has enabled the robot.
   *
   * @return True if the robot is enabled and the DS is connected
   */
  static bool IsEnabled() {
    hal::ControlWord controlWord = hal::GetControlWord();
    return controlWord.IsEnabled() && controlWord.IsDSAttached();
  }

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  static bool IsDisabled() { return !IsEnabled(); }

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  static bool IsEStopped() { return hal::GetControlWord().IsEStopped(); }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or
   * disabled.
   *
   * @return robot mode
   */
  static RobotMode GetRobotMode() {
    return hal::GetControlWord().GetRobotMode();
  }

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  static bool IsAutonomous() { return hal::GetControlWord().IsAutonomous(); }

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  static bool IsAutonomousEnabled() {
    return hal::GetControlWord().IsAutonomousEnabled();
  }

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  static bool IsTeleop() { return hal::GetControlWord().IsTeleop(); }

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  static bool IsTeleopEnabled() {
    return hal::GetControlWord().IsTeleopEnabled();
  }

  /**
   * Check if the DS is commanding utility mode.
   *
   * @return True if the robot is being commanded to be in utility mode
   */
  static bool IsUtility() { return hal::GetControlWord().IsUtility(); }

  /**
   * Check if the DS is commanding Utility mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in Utility mode and
   * enabled.
   */
  static bool IsUtilityEnabled() {
    return hal::GetControlWord().IsUtilityEnabled();
  }

  static std::string OpModeToString(int64_t id);

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color
   * @param backgroundColor background color
   * @return unique ID used to later identify the operating mode; if a blank
   * name is passed, 0 is returned; identical names for the same robot
   * mode result in a 0 return value
   */
  static int64_t AddOpMode(RobotMode mode, std::string_view name,
                           std::string_view group, std::string_view description,
                           const wpi::util::Color& textColor,
                           const wpi::util::Color& backgroundColor);

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @return unique ID used to later identify the operating mode; if a blank
   * name is passed, 0 is returned; identical names for the same robot
   * mode result in a 0 return value
   */
  static int64_t AddOpMode(RobotMode mode, std::string_view name,
                           std::string_view group = {},
                           std::string_view description = {});

  /**
   * Removes an operating mode option. It's necessary to call PublishOpModes()
   * to make the removed mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID for the opmode, or 0 if not found
   */
  static int64_t RemoveOpMode(RobotMode mode, std::string_view name);

  /**
   * Publishes the operating mode options to the driver station.
   */
  static void PublishOpModes();

  /**
   * Clears all operating mode options and publishes an empty list to the driver
   * station.
   */
  static void ClearOpModes();

  /**
   * Sets the program starting flag in the DS. This will also allow
   * getOpModeId() and getOpMode() to return values for the selected
   * OpMode in the DS application, if the DS is connected by the time this
   * method is called.
   *
   * <p>Most users will not need to use this method; the TimedRobot and
   * OpModeRobot robot framework classes will call it automatically after
   * the main robot class is instantiated.
   *
   * <p>This is what changes the DS to showing robot code ready.
   */
  static void ObserveUserProgramStarting();

  /**
   * Gets the operating mode selected on the driver station. Note this does not
   * mean the robot is enabled; use IsEnabled() for that. In a match, this will
   * indicate the operating mode selected for auto before the match starts
   * (i.e., while the robot is disabled in auto mode); after the auto period
   * ends, this will change to reflect the operating mode selected for teleop.
   *
   * @return the unique ID provided by the AddOpMode() function; may return 0 or
   * a unique ID not added, so callers should be prepared to handle that case
   */
  static int64_t GetOpModeId();

  /**
   * Gets the operating mode selected on the driver station. Note this does not
   * mean the robot is enabled; use IsEnabled() for that. In a match, this will
   * indicate the operating mode selected for auto before the match starts
   * (i.e., while the robot is disabled in auto mode); after the auto period
   * ends, this will change to reflect the operating mode selected for teleop.
   *
   * @return Operating mode string; may return a string not in the list of
   * options, so callers should be prepared to handle that case
   */
  static std::string GetOpMode();

  /**
   * Check to see if the selected operating mode is a particular value. Note
   * this does not mean the robot is enabled; use IsEnabled() for that.
   *
   * @param id operating mode unique ID
   * @return True if that mode is the current mode
   */
  static bool IsOpMode(int64_t id) { return GetOpModeId() == id; }

  /**
   * Check to see if the selected operating mode is a particular value. Note
   * this does not mean the robot is enabled; use IsEnabled() for that.
   *
   * @param mode operating mode
   * @return True if that mode is the current mode
   */
  static bool IsOpMode(std::string_view mode) { return GetOpMode() == mode; }

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  static bool IsDSAttached() { return hal::GetControlWord().IsDSAttached(); }

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  static bool IsFMSAttached() { return hal::GetControlWord().IsFMSAttached(); }
};

}  // namespace wpi
