/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.controller.PIDController;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
public class SetDistanceToBox extends Command {
  private final PIDController m_pid = new PIDController(-2, 0, 0);

  /**
   * Create a new set distance to box command.
   * @param distance The distance away from the box to drive to
   */
  public SetDistanceToBox(double distance) {
    requires(Robot.m_drivetrain);

    m_pid.setAbsoluteTolerance(0.01);
    m_pid.setSetpoint(distance);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    // Get everything in a safe starting state.
    Robot.m_drivetrain.reset();
    m_pid.reset();
  }

  @Override
  protected void execute() {
    double pidOut = m_pid.calculate(Robot.m_drivetrain.getDistanceToObstacle());

    Robot.m_drivetrain.drive(pidOut, pidOut);
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return m_pid.atSetpoint();
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    // Stop the wheels
    Robot.m_drivetrain.drive(0, 0);
  }
}
