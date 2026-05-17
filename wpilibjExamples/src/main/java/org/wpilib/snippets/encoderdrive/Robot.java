// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.encoderdrive;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.Spark;
import org.wpilib.hardware.rotation.Encoder;

/**
 * Encoder drive to distance snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  // Creates an encoder on DIO ports 0 and 1
  Encoder encoder = new Encoder(0, 1);
  // Initialize motor controllers and drive
  Spark leftLeader = new Spark(0);
  Spark leftFollower = new Spark(1);
  Spark rightLeader = new Spark(2);
  Spark rightFollower = new Spark(3);
  DifferentialDrive drive =
      new DifferentialDrive(leftLeader::setThrottle, rightLeader::setThrottle);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Configures the encoder's distance-per-pulse
    // The robot moves forward 1 foot per encoder rotation
    // There are 256 pulses per encoder rotation
    encoder.setDistancePerPulse(1.0 / 256.0);
    // Invert the right side of the drivetrain. You might have to invert the other side
    rightLeader.setInverted(true);
    // Configure the followers to follow the leaders
    leftLeader.addFollower(leftFollower);
    rightLeader.addFollower(rightFollower);
  }

  /** Drives forward at half velocity until the robot has moved 5 feet, then stops. */
  @Override
  public void autonomousPeriodic() {
    if (encoder.getDistance() < 5.0) {
      drive.tankDrive(0.5, 0.5);
    } else {
      drive.tankDrive(0.0, 0.0);
    }
  }
}
