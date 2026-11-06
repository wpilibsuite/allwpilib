// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.subsystems;

import org.wpilib.command2.SubsystemBase;
import org.wpilib.drive.DifferentialDrive;
import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.examples.hatchbottraditional.Constants.DriveConstants;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.telemetry.TelemetryTable;

public class DriveSubsystem extends SubsystemBase {
  // The motors on the left side of the drive.
  private final PWMSparkMax left1 = new PWMSparkMax(DriveConstants.LEFT_MOTOR1_PORT);

  // The motors on the right side of the drive.
  private final PWMSparkMax right1 = new PWMSparkMax(DriveConstants.RIGHT_MOTOR1_PORT);

  // The robot's drive
  private final DifferentialDrive drive =
      new DifferentialDrive(left1::setThrottle, right1::setThrottle);

  // The left-side drive encoder
  private final Encoder leftEncoder =
      new Encoder(
          DriveConstants.LEFT_ENCODER_PORTS[0],
          DriveConstants.LEFT_ENCODER_PORTS[1],
          DriveConstants.LEFT_ENCODER_REVERSED);

  // The right-side drive encoder
  private final Encoder rightEncoder =
      new Encoder(
          DriveConstants.RIGHT_ENCODER_PORTS[0],
          DriveConstants.RIGHT_ENCODER_PORTS[1],
          DriveConstants.RIGHT_ENCODER_REVERSED);

  /** Creates a new DriveSubsystem. */
  public DriveSubsystem() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    right1.setInverted(true);

    // Sets the distance per pulse for the encoders
    leftEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);
    rightEncoder.setDistancePerPulse(DriveConstants.ENCODER_DISTANCE_PER_PULSE);
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  public void arcadeDrive(double fwd, double rot) {
    drive.arcadeDrive(fwd, rot);
  }

  /** Resets the drive encoders to currently read a position of 0. */
  public void resetEncoders() {
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

  @Override
  public void logTo(TelemetryTable table) {
    super.logTo(table);
    // Publish encoder distances to telemetry.
    table.log("leftDistance", leftEncoder.getDistance());
    table.log("rightDistance", rightEncoder.getDistance());
  }
}
