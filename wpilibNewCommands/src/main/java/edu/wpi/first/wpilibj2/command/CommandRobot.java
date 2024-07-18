// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * CommandRobot is a wrapper over TimedRobot designed to work well with Command based programming.
 *
 * <p>The CommandRobot class is intended to be subclassed by a user when creating a robot program.
 */
public class CommandRobot extends TimedRobot {
  /** The CommandScheduler instance. */
  protected CommandScheduler m_scheduler = CommandScheduler.getInstance();

  /** The autonomous chooser. */
  protected SendableChooser<Command> m_autoChooser = new SendableChooser<>();

  private Command m_autonomousCommand;

  /** Constructor for CommandRobot. */
  protected CommandRobot() {
    this(kDefaultPeriod);
  }

  /**
   * Constructor for CommandRobot.
   *
   * @param period Period in seconds.
   */
  protected CommandRobot(double period) {
    super(period);
    addPeriodic(() -> m_scheduler.run(), 0.02);
    m_autoChooser.setDefaultOption(
        "No autos configured.",
        Commands.print("No autos configured. Add Commands to m_autoChooser to fix this."));
    SmartDashboard.putData(m_autoChooser);
  }

  @Override
  public final void robotInit() {}

  @Override
  public final void robotPeriodic() {}

  @Override
  public void autonomousInit() {
    m_autonomousCommand = m_autoChooser.getSelected();
    if (m_autonomousCommand != null) {
      m_scheduler.schedule(m_autonomousCommand);
    }
  }

  @Override
  public final void autonomousPeriodic() {}

  @Override
  public void autonomousExit() {}

  @Override
  public final void teleopInit() {
    if (m_autonomousCommand != null) {
      m_scheduler.cancel(m_autonomousCommand);
    }
  }

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
