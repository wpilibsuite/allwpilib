// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;

/**
 * Class for odometry. Robot code should not use this directly- Instead, use the particular type for
 * your drivetrain (e.g., {@link DifferentialDriveOdometry}). Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from encoders and a
 * gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 *
 * @param <T> Wheel positions type.
 */
public class Odometry<T> {
  private final Kinematics<?, ?, T> m_kinematics;
  private Pose2d m_pose;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;
  private final T m_previousWheelPositions;

  /**
   * Constructs an Odometry object.
   *
   * @param kinematics The kinematics of the drivebase.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @param initialPose The starting position of the robot on the field.
   */
  public Odometry(
      Kinematics<?, ?, T> kinematics, Rotation2d gyroAngle, T wheelPositions, Pose2d initialPose) {
    m_kinematics = kinematics;
    m_pose = initialPose;
    m_gyroOffset = m_pose.getRotation().minus(gyroAngle);
    m_previousAngle = m_pose.getRotation();
    m_previousWheelPositions = m_kinematics.copy(wheelPositions);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(Rotation2d gyroAngle, T wheelPositions, Pose2d pose) {
    m_pose = pose;
    m_previousAngle = m_pose.getRotation();
    m_gyroOffset = m_pose.getRotation().minus(gyroAngle);
    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
  }

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  public void resetPose(Pose2d pose) {
    m_gyroOffset = m_gyroOffset.plus(pose.getRotation().minus(m_pose.getRotation()));
    m_pose = pose;
    m_previousAngle = m_pose.getRotation();
  }

  /**
   * Resets the translation of the pose.
   *
   * @param translation The translation to reset to.
   */
  public void resetTranslation(Translation2d translation) {
    m_pose = new Pose2d(translation, m_pose.getRotation());
  }

  /**
   * Resets the rotation of the pose.
   *
   * @param rotation The rotation to reset to.
   */
  public void resetRotation(Rotation2d rotation) {
    m_gyroOffset = m_gyroOffset.plus(rotation.minus(m_pose.getRotation()));
    m_pose = new Pose2d(m_pose.getTranslation(), rotation);
    m_previousAngle = m_pose.getRotation();
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x and y are in meters).
   */
  public Pose2d getPose() {
    return m_pose;
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time. This method takes in an angle parameter which is used instead of the angular rate
   * that is calculated from forward kinematics, in addition to the current distance measurement at
   * each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, T wheelPositions) {
    var angle = gyroAngle.plus(m_gyroOffset);

    var twist = m_kinematics.toTwist2d(m_previousWheelPositions, wheelPositions);
    twist.dtheta = angle.minus(m_previousAngle).getRadians();

    var newPose = m_pose.exp(twist);

    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
    m_previousAngle = angle;
    m_pose = new Pose2d(newPose.getTranslation(), angle);

    return m_pose;
  }
}
