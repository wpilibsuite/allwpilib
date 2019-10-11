/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.differentialdriveramsete;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.SpeedControllerGroup;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

/**
 * Represents a differential drive style drivetrain.
 */
public class Drivetrain extends SubsystemBase {
  public static final double kMaxSpeed = 3.0; // meters per second
  public static final double kMaxAngularSpeed = 2 * Math.PI; // one rotation per second

  private static final double kTrackWidth = 0.381 * 2; // meters
  private static final double kWheelRadius = 0.0508; // meters
  private static final int kEncoderResolution = 4096;

  // Motors
  private final Spark m_leftMaster = new Spark(1);
  private final Spark m_leftFollower = new Spark(2);
  private final Spark m_rightMaster = new Spark(3);
  private final Spark m_rightFollower = new Spark(4);

  // Encoders
  private final Encoder m_leftEncoder = new Encoder(0, 1);
  private final Encoder m_rightEncoder = new Encoder(2, 3);

  // Speed Controller Groups
  private final SpeedControllerGroup m_leftGroup
      = new SpeedControllerGroup(m_leftMaster, m_leftFollower);
  private final SpeedControllerGroup m_rightGroup
      = new SpeedControllerGroup(m_rightMaster, m_rightFollower);

  // Gyro
  private final AnalogGyro m_gyro = new AnalogGyro(0);

  // PID Controllers to track individual velocities
  private final PIDController m_leftPIDController = new PIDController(1, 0, 0);
  private final PIDController m_rightPIDController = new PIDController(1, 0, 0);

  // Kinematics object to convert between chassis speeds and wheel speeds
  private final DifferentialDriveKinematics m_kinematics
      = new DifferentialDriveKinematics(kTrackWidth);

  // Odometry to track field relative pose
  private final DifferentialDriveOdometry m_odometry
      = new DifferentialDriveOdometry(m_kinematics);

  // The current pose.
  private Pose2d m_pose = new Pose2d();

  /**
   * Constructs a differential drive object.
   * Sets the encoder distance per pulse and resets the gyro.
   */
  public Drivetrain() {
    m_gyro.reset();

    // Set the distance per pulse for the drive encoders. We can simply use the
    // distance traveled for one rotation of the wheel divided by the encoder
    // resolution.
    m_leftEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);
    m_rightEncoder.setDistancePerPulse(2 * Math.PI * kWheelRadius / kEncoderResolution);
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
   * Returns the current wheel speeds.
   *
   * @return The current wheel speeds.
   */
  public DifferentialDriveWheelSpeeds getCurrentSpeeds() {
    return new DifferentialDriveWheelSpeeds(m_leftEncoder.getRate(), m_rightEncoder.getRate());
  }

  /**
   * Returns the current pose.
   *
   * @return The current pose.
   */
  public Pose2d getPose() {
    return m_pose;
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
   * Sets the desired chassis speeds.
   *
   * @param speeds The desired chassis speeds.
   */
  public void setSpeeds(ChassisSpeeds speeds) {
    setSpeeds(m_kinematics.toWheelSpeeds(speeds));
  }

  public void reset(Pose2d pose) {
    m_gyro.reset();
    m_odometry.resetPosition(pose);
  }

  /**
   * Updates the field-relative position.
   */
  @Override
  public void periodic() {
    m_pose = m_odometry.update(getAngle(), getCurrentSpeeds());
  }
}
