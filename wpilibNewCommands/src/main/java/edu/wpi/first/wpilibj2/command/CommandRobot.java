// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import edu.wpi.first.wpilibj2.command.button.RobotModeTriggers;

/**
 * CommandRobot is a wrapper over TimedRobot designed to work well with Command based programming.
 *
 * <p>The CommandRobot class is intended to be subclassed by a user when creating a robot program.
 */
public class CommandRobot extends TimedRobot {
  /** The CommandScheduler instance. */
  protected CommandScheduler m_scheduler = CommandScheduler.getInstance();

  /**
   * A trigger that is true when the robot is enabled in autonomous mode.
   */
  protected Trigger autonomous = RobotModeTriggers.autonomous();

  /**
   * A trigger that is true when the robot is enabled in teleop mode.
   */
  protected Trigger teleop = RobotModeTriggers.teleop();

  /**
   * A trigger that is true when the robot is disabled.
   */
  protected Trigger disabled = RobotModeTriggers.disabled();

  /**
   * A trigger that is true when the robot is enabled in test mode.
   */
  protected Trigger test = RobotModeTriggers.test();

  /** Constructor for CommandRobot. */
  protected CommandRobot() {
    this(kDefaultPeriod);
  }

  /**
   * Constructor for CommandRobot.
   *
   * @param period Period in seconds.
   */
  @SuppressWarnings("this-escape")
  protected CommandRobot(double period) {
    super(period);
    addPeriodic(() -> m_scheduler.run(), 0.02);
  }

  @Override
  public final void robotInit() {}

  @Override
  public final void robotPeriodic() {}

  @Override
  public void autonomousInit() {}

  @Override
  public final void autonomousPeriodic() {}

  @Override
  public void autonomousExit() {}

  @Override
  public final void teleopInit() {}

  @Override
  public final void teleopPeriodic() {}

  @Override
  public final void teleopExit() {}

  @Override
  public final void disabledInit() {}

  @Override
  public final void disabledPeriodic() {}

  @Override
  public final void disabledExit() {}

  @Override
  public void testInit() {
    m_scheduler.cancelAll();
  }

  @Override
  public void testPeriodic() {}

  @Override
  public void testExit() {}
}
