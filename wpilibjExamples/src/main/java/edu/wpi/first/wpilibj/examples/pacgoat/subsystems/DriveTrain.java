/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.subsystems;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kFrontLeftCIMPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kFrontRightCIMPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kLeftEncoderEncodingType;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kLeftEncoderPorts;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kLeftEncoderReversed;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRealEncoderDistancePerPulse;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRearLeftCIMPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRearRightCIMPort;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRightEncoderEncodingType;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRightEncoderPorts;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kRightEncoderReversed;
import static edu.wpi.first.wpilibj.examples.pacgoat.Constants.DriveConstants.kSimEncoderDistancePerPulse;

/**
 * The DriveTrain subsystem controls the robot's chassis and reads in
 * information about it's speed and position.
 */
public class DriveTrain extends SubsystemBase {
  // Subsystem devices
  private final SpeedController m_frontLeftCIM = new Victor(kFrontLeftCIMPort);
  private final SpeedController m_frontRightCIM = new Victor(kFrontRightCIMPort);
  private final SpeedController m_rearLeftCIM = new Victor(kRearLeftCIMPort);
  private final SpeedController m_rearRightCIM = new Victor(kRearRightCIMPort);
  private final SpeedControllerGroup m_leftCIMs = new SpeedControllerGroup(
      m_frontLeftCIM, m_rearLeftCIM);
  private final SpeedControllerGroup m_rightCIMs = new SpeedControllerGroup(
      m_frontRightCIM, m_rearRightCIM);
  private final DifferentialDrive m_drive;
  private final Encoder m_rightEncoder = new Encoder(kRightEncoderPorts[0], kRightEncoderPorts[1],
      kRightEncoderReversed, kRightEncoderEncodingType);
  private final Encoder m_leftEncoder = new Encoder(kLeftEncoderPorts[0], kLeftEncoderPorts[1],
      kLeftEncoderReversed, kLeftEncoderEncodingType);
  private final AnalogGyro m_gyro = new AnalogGyro(0);

  /**
   * Create a new drive train subsystem.
   */
  public DriveTrain() {
    // Configure drive motors
    addChild("Front Left CIM", (Victor) m_frontLeftCIM);
    addChild("Front Right CIM", (Victor) m_frontRightCIM);
    addChild("Back Left CIM", (Victor) m_rearLeftCIM);
    addChild("Back Right CIM", (Victor) m_rearRightCIM);

    // Configure the DifferentialDrive to reflect the fact that all motors
    // are wired backwards (right is inverted in DifferentialDrive).
    m_leftCIMs.setInverted(true);
    m_drive = new DifferentialDrive(m_leftCIMs, m_rightCIMs);
    m_drive.setSafetyEnabled(true);
    m_drive.setExpiration(0.1);
    m_drive.setMaxOutput(1.0);

    if (Robot.isReal()) { // Converts to feet
      m_rightEncoder.setDistancePerPulse(kRealEncoderDistancePerPulse);
      m_leftEncoder.setDistancePerPulse(kRealEncoderDistancePerPulse);
    } else {
      // Convert to feet 4in diameter wheels with 360 tick sim encoders
      m_rightEncoder.setDistancePerPulse(kSimEncoderDistancePerPulse);
      m_leftEncoder.setDistancePerPulse(kSimEncoderDistancePerPulse);
    }

    addChild("Right Encoder", m_rightEncoder);
    addChild("Left Encoder", m_leftEncoder);

    // Configure gyro
    if (Robot.isReal()) {
      m_gyro.setSensitivity(0.007); // TODO: Handle more gracefully?
    }
    addChild("Gyro", m_gyro);
  }

  /**
   * Tank drive using a PS3 joystick.
   *
   * @param joy PS3 style joystick to use as the input for tank drive.
   */
  public void tankDrive(Joystick joy) {
    m_drive.tankDrive(joy.getY(), joy.getRawAxis(4));
  }

  /**
   * Tank drive using individual joystick axes.
   *
   * @param leftAxis Left sides value
   * @param rightAxis Right sides value
   */
  public void tankDrive(double leftAxis, double rightAxis) {
    m_drive.tankDrive(leftAxis, rightAxis);
  }

  /**
   * Stop the drivetrain from moving.
   */
  public void stop() {
    m_drive.tankDrive(0, 0);
  }

  /**
   * The encoder getting the distance and speed of left side of the
   * drivetrain.
   */
  public Encoder getLeftEncoder() {
    return m_leftEncoder;
  }

  /**
   * The encoder getting the distance and speed of right side of the
   * drivetrain.
   */
  public Encoder getRightEncoder() {
    return m_rightEncoder;
  }

  /**
   * The current angle of the drivetrain as measured by the Gyro.
   */
  public double getAngle() {
    return m_gyro.getAngle();
  }
}
