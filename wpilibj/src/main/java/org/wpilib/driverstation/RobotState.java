// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.util.Color;

/** Provides access to robot state information from the Driver Station. */
public final class RobotState {
  private RobotState() {}

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public static boolean isEnabled() {
    return DriverStationBackend.isEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be disabled.
   *
   * @return True if the robot should be disabled, false otherwise.
   */
  public static boolean isDisabled() {
    return DriverStationBackend.isDisabled();
  }

  /**
   * Gets a value indicating whether the Robot is e-stopped.
   *
   * @return True if the robot is e-stopped, false otherwise.
   */
  public static boolean isEStopped() {
    return DriverStationBackend.isEStopped();
  }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or disabled.
   *
   * @return robot mode
   */
  public static RobotMode getRobotMode() {
    return DriverStationBackend.getRobotMode();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public static boolean isAutonomous() {
    return DriverStationBackend.isAutonomous();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public static boolean isAutonomousEnabled() {
    return DriverStationBackend.isAutonomousEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public static boolean isTeleop() {
    return DriverStationBackend.isTeleop();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public static boolean isTeleopEnabled() {
    return DriverStationBackend.isTeleopEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public static boolean isTest() {
    return DriverStationBackend.isTest();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Test
   * mode and enabled.
   *
   * @return True if test mode should be set and the robot should be enabled.
   */
  public static boolean isTestEnabled() {
    return DriverStationBackend.isTestEnabled();
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color, or null for default
   * @param backgroundColor background color, or null for default
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same robot mode
   *     and name already exists
   */
  public static long addOpMode(
      RobotMode mode,
      String name,
      String group,
      String description,
      Color textColor,
      Color backgroundColor) {
    return DriverStationBackend.addOpMode(
        mode, name, group, description, textColor, backgroundColor);
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name, String group, String description) {
    return DriverStationBackend.addOpMode(mode, name, group, description);
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name, String group) {
    return DriverStationBackend.addOpMode(mode, name, group);
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name) {
    return DriverStationBackend.addOpMode(mode, name);
  }

  /**
   * Removes an operating mode option. It's necessary to call publishOpModes() to make the removed
   * mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID for the opmode, or 0 if not found
   */
  public static long removeOpMode(RobotMode mode, String name) {
    return DriverStationBackend.removeOpMode(mode, name);
  }

  /** Publishes the operating mode options to the driver station. */
  public static void publishOpModes() {
    DriverStationBackend.publishOpModes();
  }

  /** Clears all operating mode options and publishes an empty list to the driver station. */
  public static void clearOpModes() {
    DriverStationBackend.clearOpModes();
  }

  /**
   * Gets the operating mode selected on the driver station. Note this does not mean the robot is
   * enabled; use isEnabled() for that. In a match, this will indicate the operating mode selected
   * for auto before the match starts (i.e., while the robot is disabled in auto mode); after the
   * auto period ends, this will change to reflect the operating mode selected for teleop.
   *
   * @return the unique ID provided by the addOpMode() function; may return 0 or a unique ID not
   *     added, so callers should be prepared to handle that case
   */
  public static long getOpModeId() {
    return DriverStationBackend.getOpModeId();
  }

  /**
   * Gets the operating mode selected on the driver station. Note this does not mean the robot is
   * enabled; use isEnabled() for that. In a match, this will indicate the operating mode selected
   * for auto before the match starts (i.e., while the robot is disabled in auto mode); after the
   * auto period ends, this will change to reflect the operating mode selected for teleop.
   *
   * @return Operating mode string; may return a string not in the list of options, so callers
   *     should be prepared to handle that case
   */
  public static String getOpMode() {
    return DriverStationBackend.getOpMode();
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @param id operating mode unique ID
   * @return True if that mode is the current mode
   */
  public static boolean isOpMode(long id) {
    return DriverStationBackend.isOpMode(id);
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @param mode operating mode
   * @return True if that mode is the current mode
   */
  public static boolean isOpMode(String mode) {
    return DriverStationBackend.isOpMode(mode);
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public static boolean isDSAttached() {
    return DriverStationBackend.isDSAttached();
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public static boolean isFMSAttached() {
    return DriverStationBackend.isFMSAttached();
  }
}
