// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.Meters;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.geometry.Twist3d;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.UsageReporting;

/**
 * Class for double follower wheel odometry. Odometry allows you to track the robot's position on
 * the field over the course of a match using readings from 2 encoders and a gyroscope.
 *
 * <p>This class is meant to be an easy replacement for {@link TwoDeadWheelOdometry}, only requiring
 * the addition of appropriate conversions between 2D and 3D versions of geometry classes. (See
 * {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class TwoDeadWheelOdometry3d {
  /*
   * This class uses logic cloned from Odometry3d.java. It does not extend Odometry3d because the
   * structure of the kinematics and odometry classes assume that the gyro angle is independent from
   * the kinematics, while in a two-dead-wheel configuration, the gyro is required to do the
   * kinematics, as otherwise the forward kinematics is rank-deficient.
   */

  private final double m_xWheelYPos;
  private final double m_yWheelXPos;

  private Pose3d m_pose;

  private Rotation3d m_previousGyroAngle;

  private double m_previousXWheelPos;
  private double m_previousYWheelPos;

  /**
   * Constructs a TwoDeadWheelOdometry object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param initialPose The starting position of the robot on the field.
   */
  public TwoDeadWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      double xWheelPos,
      double yWheelPos,
      Rotation3d gyroAngle,
      Pose3d initialPose) {
    m_xWheelYPos = xWheelYPos;
    m_yWheelXPos = yWheelXPos;
    m_previousGyroAngle = gyroAngle;
    m_previousXWheelPos = xWheelPos;
    m_previousYWheelPos = yWheelPos;
    m_pose = initialPose;

    UsageReporting.reportUsage("TwoDeadWheelOdometry3d", "");
  }

  /**
   * Constructs a TwoDeadWheelOdometry object with the default pose at the origin.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   */
  public TwoDeadWheelOdometry3d(
      double xWheelYPos,
      double yWheelXPos,
      double xWheelPos,
      double yWheelPos,
      Rotation3d gyroAngle) {
    this(xWheelYPos, yWheelXPos, xWheelPos, yWheelPos, gyroAngle, Pose3d.ZERO);
  }

  /**
   * Constructs a TwoDeadWheelOdometry object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param initialPose The starting position of the robot on the field.
   */
  public TwoDeadWheelOdometry3d(
      Distance xWheelYPos,
      Distance yWheelXPos,
      Distance xWheelPos,
      Distance yWheelPos,
      Rotation3d gyroAngle,
      Pose3d initialPose) {
    this(
        xWheelYPos.in(Meters),
        yWheelXPos.in(Meters),
        xWheelPos.in(Meters),
        yWheelPos.in(Meters),
        gyroAngle,
        initialPose);
  }

  /**
   * Constructs a TwoDeadWheelOdometry object with the default pose at the origin.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   */
  public TwoDeadWheelOdometry3d(
      Distance xWheelYPos,
      Distance yWheelXPos,
      Distance xWheelPos,
      Distance yWheelPos,
      Rotation3d gyroAngle) {
    this(xWheelYPos, yWheelXPos, xWheelPos, yWheelPos, gyroAngle, Pose3d.ZERO);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here in the user's robot code.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @param pose The new position of the robot on the field.
   */
  public void resetPosition(double xWheelPos, double yWheelPos, Rotation3d gyroAngle, Pose3d pose) {
    m_previousGyroAngle = gyroAngle;
    m_previousXWheelPos = xWheelPos;
    m_previousYWheelPos = yWheelPos;
    m_pose = pose;
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
   * Updates the robot's position.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to be offset to match
   *     the robot's orientation on the field.
   * @return The updated pose.
   */
  public Pose3d update(double xWheelPos, double yWheelPos, Rotation3d gyroAngle) {
    var deltaAngle = gyroAngle.relativeTo(m_previousGyroAngle);
    var angle_difference = deltaAngle.toVector();

    final var deltaTheta = deltaAngle.toRotation2d().getRadians();
    final var deltaX = xWheelPos - m_previousXWheelPos + deltaTheta * m_xWheelYPos;
    final var deltaY = yWheelPos - m_previousYWheelPos - deltaTheta * m_yWheelXPos;

    var twist =
        new Twist3d(
            deltaX,
            deltaY,
            0,
            angle_difference.get(0),
            angle_difference.get(1),
            angle_difference.get(2));

    m_pose = m_pose.plus(twist.exp());

    m_previousGyroAngle = gyroAngle;
    m_previousXWheelPos = xWheelPos;
    m_previousYWheelPos = yWheelPos;

    return m_pose;
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse kinematics.
   *
   * @param vx The velocity of the forward-facing wheel, in meters per second.
   * @param vy The velocity of the sideways-facing wheel, in meters per second.
   * @param omega The angular velocity of the robot as reported by the gyro, in radians per second.
   * @return The velocity of the chassis.
   */
  public ChassisVelocities toChassisVelocities(double vx, double vy, double omega) {
    return new ChassisVelocities(vx + omega * m_xWheelYPos, vy - omega * m_yWheelXPos, omega);
  }
}
