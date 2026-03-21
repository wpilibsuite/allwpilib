// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.xrpreference.commands;

import org.wpilib.command2.Command;
import org.wpilib.examples.xrpreference.subsystems.Drivetrain;

public class TurnDegrees extends Command {
  private final Drivetrain drive;
  private final double degrees;
  private final double velocity;

  /**
   * Creates a new TurnDegrees. This command will turn your robot for a desired rotation (in
   * degrees) and rotational velocity.
   *
   * @param velocity The velocity which the robot will drive. Negative is in reverse.
   * @param degrees Degrees to turn. Leverages encoders to compare distance.
   * @param drive The drive subsystem on which this command will run
   */
  public TurnDegrees(double velocity, double degrees, Drivetrain drive) {
    this.degrees = degrees;
    this.velocity = velocity;
    this.drive = drive;
    addRequirements(drive);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
    // Set motors to stop, read encoder values for starting point
    drive.arcadeDrive(0, 0);
    drive.resetEncoders();
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    drive.arcadeDrive(0, velocity);
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    drive.arcadeDrive(0, 0);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    /* Need to convert distance travelled to degrees. The Standard
       XRP Chassis found here, https://www.sparkfun.com/products/22230,
       has a wheel placement diameter (163 mm) - width of the wheel (8 mm) = 155 mm
       or 6.102 inches. We then take into consideration the width of the tires.
    */
    double inchPerDegree = Math.PI * 6.102 / 360;
    // Compare distance travelled from start to distance based on degree turn
    return getAverageTurningDistance() >= (inchPerDegree * degrees);
  }

  private double getAverageTurningDistance() {
    double leftDistance = Math.abs(drive.getLeftDistanceInch());
    double rightDistance = Math.abs(drive.getRightDistanceInch());
    return (leftDistance + rightDistance) / 2.0;
  }
}
