// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.kinematics.TwoDeadWheelOdometry3d;
import org.wpilib.math.kinematics.TwoDeadWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;

/**
 * This class wraps {@link TwoDeadWheelOdometry3d Double Follower Wheel Odometry} to fuse
 * latency-compensated vision measurements with mecanum drive encoder distance measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be a drop-in replacement for
 * {@link TwoDeadWheelOdometry3d}. It is also intended to be an easy replacement for {@link
 * TwoDeadWheelPoseEstimator}, only requiring the addition of a standard deviation for Z and
 * appropriate conversions between 2D and 3D versions of geometry classes. (See {@link
 * Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link TwoDeadWheelPoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link TwoDeadWheelPoseEstimator3d#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave mostly like regular encoder odometry.
 */
public class TwoDeadWheelPoseEstimator3d extends PoseEstimator3d<TwoDeadWheelPositions> {
  /**
   * Constructs a TwoDeadWheelPoseEstimator with default standard deviations for the model and
   * vision measurements.
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
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   */
  public TwoDeadWheelPoseEstimator3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      TwoDeadWheelPositions wheelPositions,
      Pose3d initialPose) {
    this(
        xWheelYPos,
        yWheelXPos,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9, 0.9));
  }

  /**
   * Constructs a TwoDeadWheelPoseEstimator.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public TwoDeadWheelPoseEstimator3d(
      double xWheelYPos,
      double yWheelXPos,
      Rotation3d gyroAngle,
      TwoDeadWheelPositions wheelPositions,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        new TwoDeadWheelOdometry3d(xWheelYPos, yWheelXPos, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
