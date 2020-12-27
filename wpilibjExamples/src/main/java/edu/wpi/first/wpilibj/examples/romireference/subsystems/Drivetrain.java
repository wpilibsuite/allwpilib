/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.romireference.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import static edu.wpi.first.wpilibj.examples.romireference.Constants.*;

public class Drivetrain extends SubsystemBase {
  // The Romi has the left and right motors set to
  // PWM channels 0 and 1 respectively
  private final Spark m_leftMotor = new Spark(0);
  private final Spark m_rightMotor = new Spark(1);

  // The Romi has onboard encoders that are hardcoded
  // to use DIO pins 4/5 and 6/7 for the left and right
  private final Encoder m_leftEncoder = new Encoder(4, 5);
  private final Encoder m_rightEncoder = new Encoder(6, 7);

  // Set up the differential drive controller
  private final DifferentialDrive m_diffDrive = new DifferentialDrive(m_leftMotor, m_rightMotor);

  /**
   * Creates a new Drivetrain.
   */
  public Drivetrain() {
    resetEncoders();
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param xaxisSpeed the commanded forward movement
   * @param zaxisRotate the commanded rotation
   */
  public void arcadeDrive(double xaxisSpeed, double zaxisRotate) {
    m_diffDrive.arcadeDrive(xaxisSpeed, zaxisRotate);
  }

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  public void resetEncoders() {
    m_leftEncoder.reset();
    m_rightEncoder.reset();
  }

  /**
   * Gets the left drive encoder count.
   *
   * @return the left drive encoder count
   */
  public int getLeftEncoderCount() {
    return m_leftEncoder.get();
  }

  /**
   * Gets the right drive encoder count.
   *
   * @return the right drive encoder count
   */
  public int getRightEncoderCount() {
    return m_rightEncoder.get();
  }

  /**
  * Gets the left distance driven.
  *
  * @return the left-side distance driven, in inches
  */
  public double getLeftDistanceInch() {
    return Math.PI * kWheelDiameterInch * (getLeftEncoderCount() / kCountsPerRevolution);
  }
  /**
  * Gets the right distance driven.
  *
  * @return the right-side distance driven, in inches
  */
  public double getRightDistanceInch() {
    return Math.PI * kWheelDiameterInch * (getRightEncoderCount() / kCountsPerRevolution);
  }

  @Override
  public void periodic() {
    // This method will be called once per scheduler run
  }
}
