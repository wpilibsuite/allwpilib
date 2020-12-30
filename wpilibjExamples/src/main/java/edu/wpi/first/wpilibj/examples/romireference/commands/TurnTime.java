// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.romireference.commands;

import edu.wpi.first.wpilibj.examples.romireference.subsystems.Drivetrain;
import edu.wpi.first.wpilibj2.command.CommandBase;

/*
 * Creates a new TurnTime command. This command will turn your robot for a
 * desired rotational speed and time.
 */
public class TurnTime extends CommandBase {
  private final double m_duration;
  private final double m_rotationalSpeed;
  private final Drivetrain m_drive;
  private long m_startTime;

  /**
   * Creates a new TurnTime.
   *
   * @param speed The speed which the robot will turn. Negative is in reverse.
   * @param time How much time to turn in seconds
   * @param drive The drive subsystem on which this command will run
   */
  public TurnTime(double speed, double time, Drivetrain drive) {
    m_rotationalSpeed = speed;
    m_duration = time * 1000;
    m_drive = drive;
    addRequirements(drive);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
    m_startTime = System.currentTimeMillis();
    m_drive.arcadeDrive(0, 0);
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    m_drive.arcadeDrive(0, m_rotationalSpeed);
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    m_drive.arcadeDrive(0, 0);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return (System.currentTimeMillis() - m_startTime) >= m_duration;
  }
}
