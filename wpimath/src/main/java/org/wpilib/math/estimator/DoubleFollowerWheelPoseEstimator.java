// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.DoubleFollowerWheelOdometry;
import org.wpilib.math.kinematics.DoubleFollowerWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

public class DoubleFollowerWheelPoseEstimator extends PoseEstimator<DoubleFollowerWheelPositions> {
  /**
   * Constructs a DoubleFollowerWheelPoseEstimator with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * and 0.1 radians for heading. The default standard deviations of the vision measurements are 0.9
   * meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current distance measurements and rotations of the swerve modules.
   * @param initialPose The starting pose estimate.
   */
  public DoubleFollowerWheelPoseEstimator(
      double xWheelYPos,
      double yWheelXPos,
      Rotation2d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        xWheelYPos,
        yWheelXPos,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  /**
   * Constructs a DoubleFollowerWheelPoseEstimator.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
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
  public DoubleFollowerWheelPoseEstimator(
      double xWheelYPos,
      double yWheelXPos,
      Rotation2d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        new DoubleFollowerWheelOdometry(
            xWheelYPos, yWheelXPos, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
