package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.DriverStation;

/**
 * A class containing static {@link Trigger} factories for running callbacks when the robot mode
 * changes.
 */
public class GameTriggers {

  // Utility class
  private GameTriggers() {}

  /**
   * Returns a trigger that is true when the robot is enabled in autonomous mode.
   *
   * @return A trigger that is true when the robot is enabled in autonomous mode.
   */
  public static Trigger autonomous() {
    return new Trigger(DriverStation::isAutonomousEnabled);
  }

  /**
   * Returns a trigger that is true when the robot is enabled in teleop mode.
   *
   * @return A trigger that is true when the robot is enabled in teleop mode.
   */
  public static Trigger teleop() {
    return new Trigger(DriverStation::isTeleopEnabled);
  }

  /**
   * Returns a trigger that is true when the robot is disabled.
   *
   * @return A trigger that is true when the robot is disabled.
   */
  public static Trigger disabled() {
    return new Trigger(DriverStation::isDisabled);
  }

  /**
   * Returns a trigger that is true when the robot is enabled in test mode.
   *
   * @return A trigger that is true when the robot is enabled in test mode.
   */
  public static Trigger test() {
    return new Trigger(DriverStation::isTestEnabled);
  }
}
