// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotcmdv3.mechanisms;

import java.util.function.DoubleSupplier;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.drive.DifferentialDrive;
import org.wpilib.examples.hatchbotcmdv3.Constants.DriveConstants;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.util.sendable.SendableRegistry;

public class DriveMechanism implements Mechanism {
  // The motors on the left side of the drive.
  private final PWMSparkMax leftLeader = new PWMSparkMax(DriveConstants.kLeftMotor1Port);
  private final PWMSparkMax leftFollower = new PWMSparkMax(DriveConstants.kLeftMotor2Port);

  // The motors on the right side of the drive.
  private final PWMSparkMax rightLeader = new PWMSparkMax(DriveConstants.kRightMotor1Port);
  private final PWMSparkMax rightFollower = new PWMSparkMax(DriveConstants.kRightMotor2Port);

  // The robot's drive
  private final DifferentialDrive drive =
      new DifferentialDrive(leftLeader::setThrottle, rightLeader::setThrottle);

  // The left-side drive encoder
  private final Encoder leftEncoder =
      new Encoder(
          DriveConstants.kLeftEncoderPorts[0],
          DriveConstants.kLeftEncoderPorts[1],
          DriveConstants.kLeftEncoderReversed);

  // The right-side drive encoder
  private final Encoder rightEncoder =
      new Encoder(
          DriveConstants.kRightEncoderPorts[0],
          DriveConstants.kRightEncoderPorts[1],
          DriveConstants.kRightEncoderReversed);

  /** Creates a new DriveMechanism. */
  public DriveMechanism() {
    SendableRegistry.addChild(drive, leftLeader);
    SendableRegistry.addChild(drive, rightLeader);

    leftLeader.addFollower(leftFollower);
    rightLeader.addFollower(rightFollower);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.setInverted(true);

    // Sets the distance per pulse for the encoders
    leftEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);
    rightEncoder.setDistancePerPulse(DriveConstants.kEncoderDistancePerPulse);
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param speed The speed to drive at, from -1 (full reverse) to +1 (full forward)
   * @param turn The turn rate, from -1 (full counterclockwise) to +1 (full clockwise)
   * @return A command that drives the robot using arcade controls
   */
  public Command arcadeDrive(DoubleSupplier speed, DoubleSupplier turn) {
    return runRepeatedly(() -> drive.arcadeDrive(speed.getAsDouble(), turn.getAsDouble()))
        .named("Arcade Drive");
  }

  /**
   * Drives a set distance, in inches, at a set speed ratio from 0-1. The command will exit when the
   * distance has been reached.
   *
   * @param distance How many inches to travel. This may be negative to drive backwards.
   * @param speed Speed ratio from 0 (off) to 1 (full speed). Negative values will make the robot
   *     drive the wrong way.
   * @return A command that drives a specified distance.
   */
  public Command driveDistance(double distance, double speed) {
    return run(coroutine -> {
          resetEncoders();

          if (distance >= 0) {
            // drive forward
            while (getAverageEncoderDistance() < distance) {
              drive.tankDrive(speed, 0);
              coroutine.yield();
            }
          } else {
            // drive backward
            while (getAverageEncoderDistance() > distance) {
              drive.tankDrive(-speed, 0);
              coroutine.yield();
            }
          }

          // Finally, stop
          drive.stopMotor();
        })
        .named("Drive Distance[" + distance + "@" + speed + "]");
  }

  /** Resets the drive encoders to currently read a position of 0. */
  private void resetEncoders() {
    leftEncoder.reset();
    rightEncoder.reset();
  }

  /**
   * Gets the average distance of the TWO encoders.
   *
   * @return the average of the TWO encoder readings
   */
  public double getAverageEncoderDistance() {
    return (leftEncoder.getDistance() + rightEncoder.getDistance()) / 2.0;
  }

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  public void setMaxOutput(double maxOutput) {
    drive.setMaxOutput(maxOutput);
  }

  //  @Override
  //  public void initSendable(SendableBuilder builder) {
  //    super.initSendable(builder);
  //    // Publish encoder distances to telemetry.
  //    builder.addDoubleProperty("leftDistance", leftEncoder::getDistance, null);
  //    builder.addDoubleProperty("rightDistance", rightEncoder::getDistance, null);
  //  }
}
