// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.TripleFollowerWheelKinematics;
import org.wpilib.math.kinematics.TripleFollowerWheelOdometry;
import org.wpilib.math.kinematics.TripleFollowerWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

/**
 * This class wraps {@link TripleFollowerWheelOdometry Triple Follower Wheel Odometry} to fuse
 * latency-compensated vision measurements with triple follower wheel encoder measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be a drop-in replacement for
 * {@link TripleFollowerWheelOdometry}.
 *
 * <p>{@link TripleFollowerWheelPoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link TripleFollowerWheelPoseEstimator#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it, then this class will behave mostly like regular encoder odometry.
 */
public class TripleFollowerWheelPoseEstimator extends PoseEstimator<TripleFollowerWheelPositions> {
  /**
   * Constructs a TripleFollowerWheelPoseEstimator with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * and 0.1 radians for heading. The default standard deviations of the vision measurements are 0.9
   * meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The starting pose estimate.
   */
  public TripleFollowerWheelPoseEstimator(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        kinematics,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  /**
   * Constructs a TripleFollowerWheelPoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param wheelPositions The distances and angles measured by the follower wheels.
   * @param initialPose The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public TripleFollowerWheelPoseEstimator(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new TripleFollowerWheelOdometry(kinematics, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
