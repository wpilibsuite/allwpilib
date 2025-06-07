// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.encoderdrive;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.motorcontrol.Spark;

/**
 * Encoder drive to distance snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  // Creates an encoder on DIO ports 0 and 1
  Encoder m_encoder = new Encoder(0, 1);
  // Initialize motor controllers and drive
  Spark m_leftLeader = new Spark(0);
  Spark m_leftFollower = new Spark(1);
  Spark m_rightLeader = new Spark(2);
  Spark m_rightFollower = new Spark(3);
  DifferentialDrive m_drive = new DifferentialDrive(m_leftLeader::set, m_rightLeader::set);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Configures the encoder's distance-per-pulse
    // The robot moves forward 1 foot per encoder rotation
    // There are 256 pulses per encoder rotation
    m_encoder.setDistancePerPulse(1.0 / 256.0);
    // Invert the right side of the drivetrain. You might have to invert the other side
    m_rightLeader.setInverted(true);
    // Configure the followers to follow the leaders
    m_leftLeader.addFollower(m_leftFollower);
    m_rightLeader.addFollower(m_rightFollower);
  }

  /** Drives forward at half speed until the robot has moved 5 feet, then stops. */
  @Override
  public void autonomousPeriodic() {
    if (m_encoder.getDistance() < 5.0) {
      m_drive.tankDrive(0.5, 0.5);
    } else {
      m_drive.tankDrive(0.0, 0.0);
    }
  }
}
