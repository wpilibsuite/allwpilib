// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ThreeDeadWheelOdometry;
import org.wpilib.math.kinematics.ThreeDeadWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

/**
 * This class wraps {@link ThreeDeadWheelOdometry Double Follower Wheel Odometry} to fuse
 * latency-compensated vision measurements with differential drive encoder measurements. It is
 * intended to be a drop-in replacement for {@link ThreeDeadWheelOdometry}; in fact, if you
 * never call {@link ThreeDeadWheelPoseEstimator#addVisionMeasurement} and only call {@link
 * ThreeDeadWheelPoseEstimator#update} then this will behave exactly the same as
 * ThreeDeadWheelOdometry.
 *
 * <p>{@link ThreeDeadWheelPoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link ThreeDeadWheelPoseEstimator#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it then this class will behave exactly like regular encoder odometry.
 */
public class ThreeDeadWheelPoseEstimator extends PoseEstimator<ThreeDeadWheelPositions> {
  /**
   * Constructs a ThreeDeadWheelPoseEstimator with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * and 0.1 radians for heading. The default standard deviations of the vision measurements are 0.9
   * meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param x1WheelYPos The y-position of the first forward-facing wheel relative to the center of
   *     the robot in meters.
   * @param x2WheelYPos The y-position of the second forward-facing wheel relative to the center of
   *     the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current distance measurements and rotations of the swerve modules.
   * @param initialPose The starting pose estimate.
   */
  public ThreeDeadWheelPoseEstimator(
      double x1WheelYPos,
      double x2WheelYPos,
      double yWheelXPos,
      Rotation2d gyroAngle,
      ThreeDeadWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        x1WheelYPos,
        x2WheelYPos,
        yWheelXPos,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  /**
   * Constructs a ThreeDeadWheelPoseEstimator.
   *
   * @param x1WheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param x2WheelYPos The y-position of the second forward-facing wheel relative to the center of
   *     the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The .
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public ThreeDeadWheelPoseEstimator(
      double x1WheelYPos,
      double x2WheelYPos,
      double yWheelXPos,
      Rotation2d gyroAngle,
      ThreeDeadWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        new ThreeDeadWheelOdometry(
            x1WheelYPos, x2WheelYPos, yWheelXPos, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
