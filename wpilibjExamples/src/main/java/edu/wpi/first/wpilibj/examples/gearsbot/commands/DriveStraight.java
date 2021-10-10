// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Drivetrain;
import edu.wpi.first.wpilibj2.command.PIDCommand;

/**
 * Drive the given distance straight (negative values go backwards). Uses a local PID controller to
 * run a simple PID loop that is only enabled while this command is running. The input is the
 * averaged values of the left and right encoders.
 */
public class DriveStraight extends PIDCommand {
  private final Drivetrain m_drivetrain;

  /**
   * Create a new DriveStraight command.
   *
   * @param distance The distance to drive
   */
  public DriveStraight(double distance, Drivetrain drivetrain) {
    super(
        new PIDController(4, 0, 0), drivetrain::getDistance, distance, d -> drivetrain.drive(d, d));

    m_drivetrain = drivetrain;
    addRequirements(m_drivetrain);

    getController().setTolerance(0.01);
  }

  // Called just before this Command runs the first time
  @Override
  public void initialize() {
    // Get everything in a safe starting state.
    m_drivetrain.reset();
    super.initialize();
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  public boolean isFinished() {
    return getController().atSetpoint();
  }
}
