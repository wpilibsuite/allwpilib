// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.geometry.Twist3d;

/**
 * Class for odometry. Robot code should not use this directly- Instead, use the particular type for
 * your drivetrain (e.g., {@link DifferentialDriveOdometry3d}). Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from encoders and a
 * gyroscope.
 *
 * <p>This class is meant to be an easy replacement for {@link Odometry}, only requiring the
 * addition of appropriate conversions between 2D and 3D versions of geometry classes. (See {@link
 * Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 *
 * @param <T> Wheel positions type.
 */
public class Odometry3d<T> {
  private final Kinematics<?, T> m_kinematics;
  private Pose3d m_poseMeters;

  private Rotation3d m_gyroOffset;
  private Rotation3d m_previousAngle;
  private final T m_previousWheelPositions;

  /**
   * Constructs an Odometry3d object.
   *
   * @param kinematics The kinematics of the drivebase.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @param initialPoseMeters The starting position of the robot on the field.
   */
  public Odometry3d(
      Kinematics<?, T> kinematics,
      Rotation3d gyroAngle,
      T wheelPositions,
      Pose3d initialPoseMeters) {
    m_kinematics = kinematics;
    m_poseMeters = initialPoseMeters;
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = m_poseMeters.getRotation();
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
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(Rotation3d gyroAngle, T wheelPositions, Pose3d poseMeters) {
    m_poseMeters = poseMeters;
    m_previousAngle = m_poseMeters.getRotation();
    m_gyroOffset = m_poseMeters.getRotation().minus(gyroAngle);
    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
  }

  /**
   * Resets the pose.
   *
   * @param poseMeters The pose to reset to.
   */
  public void resetPose(Pose3d poseMeters) {
    m_gyroOffset = m_gyroOffset.plus(poseMeters.getRotation().minus(m_poseMeters.getRotation()));
    m_poseMeters = poseMeters;
    m_previousAngle = m_poseMeters.getRotation();
  }

  /**
   * Resets the translation of the pose.
   *
   * @param translation The translation to reset to.
   */
  public void resetTranslation(Translation3d translation) {
    m_poseMeters = new Pose3d(translation, m_poseMeters.getRotation());
  }

  /**
   * Resets the rotation of the pose.
   *
   * @param rotation The rotation to reset to.
   */
  public void resetRotation(Rotation3d rotation) {
    m_gyroOffset = m_gyroOffset.plus(rotation.minus(m_poseMeters.getRotation()));
    m_poseMeters = new Pose3d(m_poseMeters.getTranslation(), rotation);
    m_previousAngle = m_poseMeters.getRotation();
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x, y, and z are in meters).
   */
  public Pose3d getPoseMeters() {
    return m_poseMeters;
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
  public Pose3d update(Rotation3d gyroAngle, T wheelPositions) {
    var angle = gyroAngle.plus(m_gyroOffset);
    var angle_difference = angle.minus(m_previousAngle).toVector();

    var twist2d = m_kinematics.toTwist2d(m_previousWheelPositions, wheelPositions);
    var twist =
        new Twist3d(
            twist2d.dx,
            twist2d.dy,
            0,
            angle_difference.get(0),
            angle_difference.get(1),
            angle_difference.get(2));

    var newPose = m_poseMeters.exp(twist);

    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
    m_previousAngle = angle;
    m_poseMeters = new Pose3d(newPose.getTranslation(), angle);

    return m_poseMeters;
  }
}
