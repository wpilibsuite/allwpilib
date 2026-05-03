// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Twist2d;

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
public abstract class Odometry<T> {
  private Pose2d m_pose;

  private Rotation2d m_gyroOffset;

  // Always equal to m_poseMeters.getRotation()
  private Rotation2d m_previousAngle;

  /**
   * Constructs an Odometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  public Odometry(Rotation2d gyroAngle, Pose2d initialPose) {
    m_pose = initialPose;
    m_gyroOffset = gyroAngle.unaryMinus().rotateBy(m_pose.getRotation());
    m_previousAngle = m_pose.getRotation();
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
  public abstract void resetPosition(Rotation2d gyroAngle, T wheelPositions, Pose2d pose);

  /**
   * Resets the robot's position on the field.
   *
   * <p>The implementing class should call this method once they have reset their wheel positions to
   * the values provided in {@link #resetPosition(Rotation2d, Object, Pose2d)}.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param pose The position on the field that your robot is at.
   */
  protected void resetPosition(Rotation2d gyroAngle, Pose2d pose) {
    m_pose = pose;
    m_gyroOffset = gyroAngle.unaryMinus().rotateBy(m_pose.getRotation());
    m_previousAngle = m_pose.getRotation();
  }

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  public void resetPose(Pose2d pose) {
    m_gyroOffset =
        m_gyroOffset.rotateBy(m_pose.getRotation().unaryMinus()).rotateBy(pose.getRotation());
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
    m_gyroOffset = m_gyroOffset.rotateBy(m_pose.getRotation().unaryMinus()).rotateBy(rotation);
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
  public abstract Pose2d update(Rotation2d gyroAngle, T wheelPositions);

  /**
   * Updates the robot's position on the field by integrating the pose over time. This protected
   * method takes in a twist, which is to be calculated by the implementing class's kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param twist The twist as calculated by the implementing class's kinematics.
   * @return The new pose of the robot.
   */
  protected Pose2d update(Rotation2d gyroAngle, Twist2d twist) {
    var angle = gyroAngle.rotateBy(m_gyroOffset);

    twist.dtheta = angle.minus(m_previousAngle).getRadians();

    var newPose = m_pose.plus(twist.exp());

    m_previousAngle = angle;
    m_pose = new Pose2d(newPose.getTranslation(), angle);

    return m_pose;
  }
}
