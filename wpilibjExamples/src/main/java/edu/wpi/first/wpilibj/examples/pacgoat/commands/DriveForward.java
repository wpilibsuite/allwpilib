/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.Command;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * This command drives the robot over a given distance with simple proportional
 * control This command will drive a given distance limiting to a maximum speed.
 */
public class DriveForward extends Command {
  private final double m_driveForwardSpeed;
  private final double m_distance;
  private double m_error;
  private static final double kTolerance = 0.1;
  private static final double kP = -1.0 / 5.0;

  public DriveForward() {
    this(10, 0.5);
  }

  public DriveForward(double dist) {
    this(dist, 0.5);
  }

  /**
   * Create a new drive forward command.
   * @param dist The distance to drive
   * @param maxSpeed The maximum speed to drive at
   */
  public DriveForward(double dist, double maxSpeed) {
    requires(Robot.drivetrain);
    m_distance = dist;
    m_driveForwardSpeed = maxSpeed;
  }

  @Override
  protected void initialize() {
    Robot.drivetrain.getRightEncoder().reset();
    setTimeout(2);
  }

  @Override
  protected void execute() {
    m_error = m_distance - Robot.drivetrain.getRightEncoder().getDistance();
    if (m_driveForwardSpeed * kP * m_error >= m_driveForwardSpeed) {
      Robot.drivetrain.tankDrive(m_driveForwardSpeed, m_driveForwardSpeed);
    } else {
      Robot.drivetrain.tankDrive(m_driveForwardSpeed * kP * m_error,
          m_driveForwardSpeed * kP * m_error);
    }
  }

  @Override
  protected boolean isFinished() {
    return Math.abs(m_error) <= kTolerance || isTimedOut();
  }

  @Override
  protected void end() {
    Robot.drivetrain.stop();
  }
}
