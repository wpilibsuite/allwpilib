/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Drive the given distance straight (negative values go backwards). Uses a
 * local PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
public class DriveStraight extends Command {
  private final PIDController m_pidController;
  private final AsynchronousControllerRunner m_pidRunner;

  /**
   * Create a new DriveStraight command.
   * @param distance The distance to drive
   */
  public DriveStraight(double distance) {
    requires(Robot.m_drivetrain);
    m_pidController = new PIDController(4, 0, 0);
    m_pidRunner = new AsynchronousControllerRunner(m_pidController, () -> distance,
        Robot.m_drivetrain::getDistance,
        output -> Robot.m_drivetrain.drive(output, output));

    m_pidController.setAbsoluteTolerance(0.01);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    // Get everything in a safe starting state.
    Robot.m_drivetrain.reset();
    m_pidController.reset();
    m_pidRunner.enable();
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return m_pidController.atReference();
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    // Stop PID and the wheels
    m_pidRunner.disable();
    Robot.m_drivetrain.drive(0, 0);
  }
}
