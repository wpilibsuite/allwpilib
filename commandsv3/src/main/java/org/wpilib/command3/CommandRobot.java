// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.wpilib.framework.OpModeRobot;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.util.Color;

/**
 * Base robot class for command-based opmode registration.
 *
 * <p>Use the {@code create*OpMode()} methods to create command-aware opmode descriptors and
 * register them with the framework in autonomous, teleoperated, or utility mode groups.
 *
 * <p>After adding opmodes, call {@link org.wpilib.driverstation.RobotState#publishOpModes()} to
 * publish the updated opmode list. Publish must be called after registration so newly added opmodes
 * are visible to the Driver Station.
 */
public abstract class CommandRobot extends OpModeRobot {
  /** Constructs a command robot with the framework default periodic loop period. */
  protected CommandRobot() {
    this(OpModeRobot.DEFAULT_PERIOD);
  }

  /**
   * Constructs a command robot with a custom periodic loop period.
   *
   * @param period Loop period in seconds.
   */
  protected CommandRobot(double period) {
    super(period);
    HAL.reportUsage("Framework", "CommandRobot");
  }

  /**
   * Creates and registers an autonomous opmode descriptor.
   *
   * <p>Call {@link org.wpilib.driverstation.RobotState#publishOpModes()} after registration so
   * newly added opmodes are visible to the Driver Station.
   *
   * @param name Display name shown in the Driver Station.
   * @return Registered command opmode descriptor.
   */
  public OpModeTriggers createAutoOpMode(String name) {
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
  public OpModeTriggers createAutoOpMode(String name, String group) {
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
  public OpModeTriggers createAutoOpMode(String name, String group, String description) {
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
  public OpModeTriggers createAutoOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    var opMode = new OpModeTriggers(name);
    addOpModeFactory(
        CommandOpMode::new,
        RobotMode.AUTONOMOUS,
        name,
        group,
        description,
        textColor,
        backgroundColor);
    return opMode;
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
  public OpModeTriggers createTeleopOpMode(String name) {
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
  public OpModeTriggers createTeleopOpMode(String name, String group) {
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
  public OpModeTriggers createTeleopOpMode(String name, String group, String description) {
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
  public OpModeTriggers createTeleopOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    var opMode = new OpModeTriggers(name);
    addOpModeFactory(
        CommandOpMode::new,
        RobotMode.TELEOPERATED,
        name,
        group,
        description,
        textColor,
        backgroundColor);
    return opMode;
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
  public OpModeTriggers createUtilityOpMode(String name) {
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
  public OpModeTriggers createUtilityOpMode(String name, String group) {
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
  public OpModeTriggers createUtilityOpMode(String name, String group, String description) {
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
  public OpModeTriggers createUtilityOpMode(
      String name, String group, String description, Color textColor, Color backgroundColor) {
    var opMode = new OpModeTriggers(name);
    addOpModeFactory(
        CommandOpMode::new,
        RobotMode.UTILITY,
        name,
        group,
        description,
        textColor,
        backgroundColor);
    return opMode;
  }
}
