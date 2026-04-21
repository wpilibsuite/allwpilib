// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "wpi/driverstation/internal/DriverStationBackend.hpp"

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
    return wpi::internal::DriverStationBackend::IsEnabled();
  }

  /**
   * Check if the robot is disabled.
   *
   * @return True if the robot is explicitly disabled or the DS is not connected
   */
  static bool IsDisabled() {
    return wpi::internal::DriverStationBackend::IsDisabled();
  }

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  static bool IsEStopped() {
    return wpi::internal::DriverStationBackend::IsEStopped();
  }

  /**
   * Gets the current robot mode.
   *
   * Note that this does not indicate whether the robot is enabled or disabled.
   *
   * @return robot mode
   */
  static RobotMode GetRobotMode() {
    return wpi::internal::DriverStationBackend::GetRobotMode();
  }

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  static bool IsAutonomous() {
    return wpi::internal::DriverStationBackend::IsAutonomous();
  }

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  static bool IsAutonomousEnabled() {
    return wpi::internal::DriverStationBackend::IsAutonomousEnabled();
  }

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  static bool IsTeleop() {
    return wpi::internal::DriverStationBackend::IsTeleop();
  }

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  static bool IsTeleopEnabled() {
    return wpi::internal::DriverStationBackend::IsTeleopEnabled();
  }

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  static bool IsTest() { return wpi::internal::DriverStationBackend::IsTest(); }

  /**
   * Check if the DS is commanding Test mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in Test mode and
   * enabled.
   */
  static bool IsTestEnabled() {
    return wpi::internal::DriverStationBackend::IsTestEnabled();
  }

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
                           const wpi::util::Color& backgroundColor) {
    return wpi::internal::DriverStationBackend::AddOpMode(
        mode, name, group, description, textColor, backgroundColor);
  }

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
                           std::string_view description = {}) {
    return wpi::internal::DriverStationBackend::AddOpMode(mode, name, group,
                                                          description);
  }

  /**
   * Removes an operating mode option. It's necessary to call PublishOpModes()
   * to make the removed mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID for the opmode, or 0 if not found
   */
  static int64_t RemoveOpMode(RobotMode mode, std::string_view name) {
    return wpi::internal::DriverStationBackend::RemoveOpMode(mode, name);
  }

  /**
   * Publishes the operating mode options to the driver station.
   */
  static void PublishOpModes() {
    wpi::internal::DriverStationBackend::PublishOpModes();
  }

  /**
   * Clears all operating mode options and publishes an empty list to the driver
   * station.
   */
  static void ClearOpModes() {
    wpi::internal::DriverStationBackend::ClearOpModes();
  }

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
  static int64_t GetOpModeId() {
    return wpi::internal::DriverStationBackend::GetOpModeId();
  }

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
  static std::string GetOpMode() {
    return wpi::internal::DriverStationBackend::GetOpMode();
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note
   * this does not mean the robot is enabled; use IsEnabled() for that.
   *
   * @param id operating mode unique ID
   * @return True if that mode is the current mode
   */
  static bool IsOpMode(int64_t id) {
    return wpi::internal::DriverStationBackend::IsOpMode(id);
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note
   * this does not mean the robot is enabled; use IsEnabled() for that.
   *
   * @param mode operating mode
   * @return True if that mode is the current mode
   */
  static bool IsOpMode(std::string_view mode) {
    return wpi::internal::DriverStationBackend::IsOpMode(mode);
  }

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  static bool IsDSAttached() {
    return wpi::internal::DriverStationBackend::IsDSAttached();
  }

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  static bool IsFMSAttached() {
    return wpi::internal::DriverStationBackend::IsFMSAttached();
  }
};

}  // namespace wpi
