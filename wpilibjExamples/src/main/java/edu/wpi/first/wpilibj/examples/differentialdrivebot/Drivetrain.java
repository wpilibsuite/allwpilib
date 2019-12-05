/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.differentialdrivebot;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;

import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kLeftMaster;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kLeftFollower;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kRightMaster;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kRightFollower;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kLeftEncoderPorts;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kRightEncoderPorts;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kLeftEncoderReversed;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kRightEncoderReversed;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kEncoderDistancePerPulse;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kDriveKinematics;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kPLeftVel;
import static edu.wpi.first.wpilibj.examples.differentialdrivebot.Constants.DriveConstants.kPRightVel;

/**
 * Represents a differential drive style drivetrain.
 */
public class Drivetrain {
  private final SpeedController m_leftMaster = new PWMVictorSPX(kLeftMaster);
  private final SpeedController m_leftFollower = new PWMVictorSPX(kLeftFollower);
  private final SpeedController m_rightMaster = new PWMVictorSPX(kRightMaster);
  private final SpeedController m_rightFollower = new PWMVictorSPX(kRightFollower);

  private final Encoder m_leftEncoder = new Encoder(kLeftEncoderPorts[0], kLeftEncoderPorts[1], kLeftEncoderReversed);
  private final Encoder m_rightEncoder = new Encoder(kRightEncoderPorts[0], kRightEncoderPorts[1], kRightEncoderReversed);

  private final SpeedControllerGroup m_leftGroup
      = new SpeedControllerGroup(m_leftMaster, m_leftFollower);
  private final SpeedControllerGroup m_rightGroup
      = new SpeedControllerGroup(m_rightMaster, m_rightFollower);

  private final AnalogGyro m_gyro = new AnalogGyro(0);

  private final PIDController m_leftPIDController = new PIDController(kPLeftVel, 0, 0);
  private final PIDController m_rightPIDController = new PIDController(kPRightVel, 0, 0);

  private final DifferentialDriveOdometry m_odometry;

  /**
   * Constructs a differential drive object.
   * Sets the encoder distance per pulse and resets the gyro.
   */
  public Drivetrain() {
    m_gyro.reset();

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    m_leftEncoder.setDistancePerPulse(kEncoderDistancePerPulse);
    m_rightEncoder.setDistancePerPulse(kEncoderDistancePerPulse);

    m_leftEncoder.reset();
    m_rightEncoder.reset();

    m_odometry = new DifferentialDriveOdometry(getAngle());
  }

  /**
   * Returns the angle of the robot as a Rotation2d.
   *
   * @return The angle of the robot.
   */
  public Rotation2d getAngle() {
    // Negating the angle because WPILib gyros are CW positive.
    return Rotation2d.fromDegrees(-m_gyro.getAngle());
  }

  /**
   * Sets the desired wheel speeds.
   *
   * @param speeds The desired wheel speeds.
   */
  public void setSpeeds(DifferentialDriveWheelSpeeds speeds) {
    double leftOutput = m_leftPIDController.calculate(m_leftEncoder.getRate(),
        speeds.leftMetersPerSecond);
    double rightOutput = m_rightPIDController.calculate(m_rightEncoder.getRate(),
        speeds.rightMetersPerSecond);
    m_leftGroup.set(leftOutput);
    m_rightGroup.set(rightOutput);
  }

  /**
   * Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xSpeed Linear velocity in m/s.
   * @param rot    Angular velocity in rad/s.
   */
  @SuppressWarnings("ParameterName")
  public void drive(double xSpeed, double rot) {
    var wheelSpeeds = kDriveKinematics.toWheelSpeeds(new ChassisSpeeds(xSpeed, 0.0, rot));
    setSpeeds(wheelSpeeds);
  }

  /**
   * Updates the field-relative position.
   */
  public void updateOdometry() {
    m_odometry.update(getAngle(), m_leftEncoder.getDistance(), m_rightEncoder.getDistance());
  }
}
