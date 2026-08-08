// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import org.wpilib.driverstation.RobotState;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.util.Color;

/**
 * Utility class for creating and registering opmode descriptors without extending a robot class.
 *
 * <p>This class provides static factory methods to create command-aware opmode descriptors and
 * register them with the framework. Use these static methods when you are not extending {@link
 * org.wpilib.framework.OpModeRobot}.
 *
 * <p>After adding opmodes, call {@link RobotState#publishOpModes()} to publish the updated opmode
 * list to the Driver Station.
 *
 * @see RobotState#publishOpModes()
 */
public final class CommandOpModes {
  private CommandOpModes() {}

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createAutoOpMode(String name) {
    return createAutoOpMode(name, "");
  }

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createAutoOpMode(String name, String group) {
    return createAutoOpMode(name, group, "");
  }

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createAutoOpMode(String name, String group, String description) {
    return createAutoOpMode(name, group, description, null, null);
  }

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createAutoOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    RobotState.addOpMode(
        RobotMode.AUTONOMOUS, name, group, description, textColor, backgroundColor);
    return new OpModeTriggers(name);
  }

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createTeleopOpMode(String name) {
    return createTeleopOpMode(name, "");
  }

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createTeleopOpMode(String name, String group) {
    return createTeleopOpMode(name, group, "");
  }

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createTeleopOpMode(String name, String group, String description) {
    return createTeleopOpMode(name, group, description, null, null);
  }

  /**
   * Creates and registers a teleoperated opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createTeleopOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    RobotState.addOpMode(
        RobotMode.TELEOPERATED, name, group, description, textColor, backgroundColor);
    return new OpModeTriggers(name);
  }

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createUtilityOpMode(String name) {
    return createUtilityOpMode(name, "");
  }

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createUtilityOpMode(String name, String group) {
    return createUtilityOpMode(name, group, "");
  }

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createUtilityOpMode(String name, String group, String description) {
    return createUtilityOpMode(name, group, description, null, null);
  }

  /**
   * Creates and registers a utility opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @param group Optional grouping/category name.
   * @param description Optional Driver Station description text.
   * @param textColor Optional text color metadata.
   * @param backgroundColor Optional background color metadata.
   * @return Registered command opmode descriptor.
   */
  public static OpModeTriggers createUtilityOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    RobotState.addOpMode(RobotMode.UTILITY, name, group, description, textColor, backgroundColor);
    return new OpModeTriggers(name);
  }
}
