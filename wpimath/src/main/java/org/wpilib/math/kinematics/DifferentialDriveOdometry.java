// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.Meters;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.util.MathSharedStore;
import org.wpilib.units.measure.Distance;

/**
 * Class for differential drive odometry. Odometry allows you to track the robot's position on the
 * field over the course of a match using readings from 2 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like path following.
 * Furthermore, odometry can be used for latency compensation when using computer-vision systems.
 */
public class DifferentialDriveOdometry extends Odometry<DifferentialDriveWheelPositions> {
  private final DifferentialDriveKinematics m_kinematics = new DifferentialDriveKinematics(1);
  private final DifferentialDriveWheelPositions m_previousWheelPositions;

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @param initialPose The starting position of the robot on the field.
   */
  public DifferentialDriveOdometry(
      Rotation2d gyroAngle, double leftDistance, double rightDistance, Pose2d initialPose) {
    super(gyroAngle, initialPose);
    m_previousWheelPositions = new DifferentialDriveWheelPositions(leftDistance, rightDistance);
    MathSharedStore.reportUsage("DifferentialDriveOdometry", "");
  }

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The starting position of the robot on the field.
   */
  public DifferentialDriveOdometry(
      Rotation2d gyroAngle, Distance leftDistance, Distance rightDistance, Pose2d initialPose) {
    this(gyroAngle, leftDistance.in(Meters), rightDistance.in(Meters), initialPose);
  }

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   */
  public DifferentialDriveOdometry(
      Rotation2d gyroAngle, double leftDistance, double rightDistance) {
    this(gyroAngle, leftDistance, rightDistance, Pose2d.kZero);
  }

  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   */
  public DifferentialDriveOdometry(
      Rotation2d gyroAngle, Distance leftDistance, Distance rightDistance) {
    this(gyroAngle, leftDistance, rightDistance, Pose2d.kZero);
  }

  @Override
  public void resetPosition(
      Rotation2d gyroAngle, DifferentialDriveWheelPositions wheelPositions, Pose2d pose) {
    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
    resetPosition(gyroAngle, pose);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, double leftDistance, double rightDistance, Pose2d pose) {
    resetPosition(
        gyroAngle, new DifferentialDriveWheelPositions(leftDistance, rightDistance), pose);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, Distance leftDistance, Distance rightDistance, Pose2d pose) {
    resetPosition(gyroAngle, leftDistance.in(Meters), rightDistance.in(Meters), pose);
  }

  @Override
  public Pose2d update(Rotation2d gyroAngle, DifferentialDriveWheelPositions wheelPositions) {
    var twist = m_kinematics.toTwist2d(m_previousWheelPositions, wheelPositions);
    m_kinematics.copyInto(wheelPositions, m_previousWheelPositions);
    return update(gyroAngle, twist);
  }

  /**
   * Updates the robot position on the field using distance measurements from encoders. This method
   * is more numerically accurate than using velocities to integrate the pose and is also
   * advantageous for teams that are using lower CPR encoders.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @return The new pose of the robot.
   */
  public Pose2d update(Rotation2d gyroAngle, double leftDistance, double rightDistance) {
    return update(gyroAngle, new DifferentialDriveWheelPositions(leftDistance, rightDistance));
  }
}
