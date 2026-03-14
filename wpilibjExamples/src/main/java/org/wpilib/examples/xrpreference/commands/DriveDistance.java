// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.xrpreference.commands;

import org.wpilib.command2.Command;
import org.wpilib.examples.xrpreference.subsystems.Drivetrain;

public class DriveDistance extends Command {
  private final Drivetrain m_drive;
  private final double m_distance;
  private final double m_velocity;

  /**
   * Creates a new DriveDistance. This command will drive your your robot for a desired distance at
   * a desired velocity.
   *
   * @param velocity The velocity at which the robot will drive
   * @param inches The number of inches the robot will drive
   * @param drive The drivetrain subsystem on which this command will run
   */
  public DriveDistance(double velocity, double inches, Drivetrain drive) {
    m_distance = inches;
    m_velocity = velocity;
    m_drive = drive;
    addRequirements(drive);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
    m_drive.arcadeDrive(0, 0);
    m_drive.resetEncoders();
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    m_drive.arcadeDrive(m_velocity, 0);
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    m_drive.arcadeDrive(0, 0);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    // Compare distance travelled from start to desired distance
    return Math.abs(m_drive.getAverageDistanceInch()) >= m_distance;
  }
}
