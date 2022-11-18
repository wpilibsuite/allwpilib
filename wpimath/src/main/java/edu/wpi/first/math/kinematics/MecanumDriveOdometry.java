// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;

/**
 * Class for mecanum drive odometry. Odometry allows you to track the robot's position on the field
 * over a course of a match using readings from your mecanum wheel encoders.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class MecanumDriveOdometry {
  private final MecanumDriveKinematics m_kinematics;
  private Pose2d m_poseMeters;
  private MecanumDriveWheelPositions m_previousWheelPositions;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  /**
   * Constructs a MecanumDriveOdometry object.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public MecanumDriveOdometry(
      MecanumDriveKinematics kinematics,
      Rotation2d gyroAngle,
      MecanumDriveWheelPositions wheelPositions,
      Pose2d initialPoseMeters) {
    m_kinematics = kinematics;
    m_poseMeters = initialPoseMeters;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_previousWheelPositions =
        new MecanumDriveWheelPositions(
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);
    MathSharedStore.reportUsage(MathUsageId.kOdometry_MecanumDrive, 1);
  }

  /**
   * Constructs a MecanumDriveOdometry object with the default pose at the origin.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   */
  public MecanumDriveOdometry(
      MecanumDriveKinematics kinematics,
      Rotation2d gyroAngle,
      MecanumDriveWheelPositions wheelPositions) {
    this(kinematics, gyroAngle, wheelPositions, new Pose2d());
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions, Pose2d poseMeters) {
    m_poseMeters = poseMeters;
    m_previousAngle = poseMeters.getRotation();
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousWheelPositions =
        new MecanumDriveWheelPositions(
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x and y are in meters).
   */
  public Pose2d getPoseMeters() {
    return m_poseMeters;
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time. This method takes in an angle parameter which is used instead of the angular rate
   * that is calculated from forward kinematics, in addition to the current distance measurement at
   * each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions) {
    var angle = gyroAngle.plus(m_gyroOffset);

    var wheelDeltas =
        new MecanumDriveWheelPositions(
            wheelPositions.frontLeftMeters - m_previousWheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters - m_previousWheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters - m_previousWheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters - m_previousWheelPositions.rearRightMeters);

    var twist = m_kinematics.toTwist2d(wheelDeltas);
    twist.dtheta = angle.minus(m_previousAngle).getRadians();
    var newPose = m_poseMeters.exp(twist);

    m_previousAngle = angle;
    m_poseMeters = new Pose2d(newPose.getTranslation(), angle);
    m_previousWheelPositions =
        new MecanumDriveWheelPositions(
            wheelPositions.frontLeftMeters,
            wheelPositions.frontRightMeters,
            wheelPositions.rearLeftMeters,
            wheelPositions.rearRightMeters);

    return m_poseMeters;
  }
}
