// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/commands2/button/OpModeTriggers.hpp"
#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/util/Color.hpp"

namespace wpi::cmd {

/**
 * Utility class for creating and registering opmode descriptors without
 * extending a robot class.
 *
 * This class provides static factory methods to create command-aware opmode
 * descriptors and register them with the framework. Use these static methods
 * when you are not extending OpModeRobot.
 *
 * After adding opmodes, call RobotState::PublishOpModes() to publish the
 * updated opmode list to the Driver Station.
 *
 * @see RobotState::PublishOpModes()
 */
class CommandOpModes {
 public:
  CommandOpModes() = delete;

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateAutoOpMode(std::string_view name);

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateAutoOpMode(std::string_view name,
                                         std::string_view group);

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateAutoOpMode(std::string_view name,
                                         std::string_view group,
                                         std::string_view description);

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateAutoOpMode(
      std::string_view name, std::string_view group,
      std::string_view description, const wpi::util::Color& textColor,
      const wpi::util::Color& backgroundColor);

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateTeleopOpMode(std::string_view name);

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateTeleopOpMode(std::string_view name,
                                           std::string_view group);

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateTeleopOpMode(std::string_view name,
                                           std::string_view group,
                                           std::string_view description);

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateTeleopOpMode(
      std::string_view name, std::string_view group,
      std::string_view description, const wpi::util::Color& textColor,
      const wpi::util::Color& backgroundColor);

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateUtilityOpMode(std::string_view name);

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateUtilityOpMode(std::string_view name,
                                            std::string_view group);

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateUtilityOpMode(std::string_view name,
                                            std::string_view group,
                                            std::string_view description);

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * Call RobotState::PublishOpModes() after registration so newly added
   * opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  static OpModeTriggers CreateUtilityOpMode(
      std::string_view name, std::string_view group,
      std::string_view description, const wpi::util::Color& textColor,
      const wpi::util::Color& backgroundColor);
};

}  // namespace wpi::cmd
