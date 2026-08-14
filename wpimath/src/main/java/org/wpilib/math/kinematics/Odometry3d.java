// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.geometry.Twist3d;

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
  private final Kinematics<T, ?, ?> m_kinematics;
  private Pose3d m_pose;

  private Rotation3d m_previousGyroAngle;

  private final T m_previousWheelPositions;

  /**
   * Constructs an Odometry3d object.
   *
   * @param kinematics The kinematics of the drivebase.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param wheelPositions The current encoder readings.
   * @param initialPose The starting position of the robot on the field.
   */
  public Odometry3d(
      Kinematics<T, ?, ?> kinematics, Rotation3d gyroAngle, T wheelPositions, Pose3d initialPose) {
    m_kinematics = kinematics;
    m_pose = initialPose;
    m_previousGyroAngle = gyroAngle;
    m_previousWheelPositions = m_kinematics.copy(wheelPositions);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here in the user's robot code.
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param wheelPositions The current encoder readings.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(Rotation3d gyroAngle, T wheelPositions, Pose3d pose) {
    m_pose = pose;
    m_previousGyroAngle = gyroAngle;
    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
  }

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  public void resetPose(Pose3d pose) {
    m_pose = pose;
  }

  /**
   * Resets the translation of the pose.
   *
   * @param translation The translation to reset to.
   */
  public void resetTranslation(Translation3d translation) {
    m_pose = new Pose3d(translation, m_pose.getRotation());
  }

  /**
   * Resets the rotation of the pose.
   *
   * @param rotation The rotation to reset to.
   */
  public void resetRotation(Rotation3d rotation) {
    m_pose = new Pose3d(m_pose.getTranslation(), rotation);
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x, y, and z are in meters).
   */
  public Pose3d getPose() {
    return m_pose;
  }

  /**
   * Updates the robot's position on the field using forward kinematics and integration of the pose
   * over time. This method takes in an angle parameter which is used instead of the angular rate
   * that is calculated from forward kinematics, in addition to the current distance measurement at
   * each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param wheelPositions The current encoder readings.
   * @return The new pose of the robot.
   */
  public Pose3d update(Rotation3d gyroAngle, T wheelPositions) {
    var angle_difference = gyroAngle.relativeTo(m_previousGyroAngle).toVector();

    var twist2d = m_kinematics.toTwist2d(m_previousWheelPositions, wheelPositions);
    var twist =
        new Twist3d(
            twist2d.dx,
            twist2d.dy,
            0,
            angle_difference.get(0),
            angle_difference.get(1),
            angle_difference.get(2));

    m_pose = m_pose.plus(twist.exp());

    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
    m_previousGyroAngle = gyroAngle;

    return m_pose;
  }
}
