// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.kinematics.DifferentialDriveWheelPositions;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;

/**
 * This class wraps {@link DifferentialDriveOdometry Differential Drive Odometry} to fuse
 * latency-compensated vision measurements with differential drive encoder measurements. It is
 * intended to be a drop-in replacement for {@link DifferentialDriveOdometry}; in fact, if you never
 * call {@link DifferentialDrivePoseEstimator#addVisionMeasurement} and only call {@link
 * DifferentialDrivePoseEstimator#update} then this will behave exactly the same as
 * DifferentialDriveOdometry.
 *
 * <p>{@link DifferentialDrivePoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link DifferentialDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it then this class will behave exactly like regular encoder odometry.
 */
public class DifferentialDrivePoseEstimator extends PoseEstimator<DifferentialDriveWheelPositions> {
  /**
   * Constructs a DifferentialDrivePoseEstimator with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.02 meters for x, 0.02 meters for
   * y, and 0.01 radians for heading. The default standard deviations of the vision measurements are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @param initialPose The starting pose estimate.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistance,
      double rightDistance,
      Pose2d initialPose) {
    this(
        kinematics,
        gyroAngle,
        leftDistance,
        rightDistance,
        initialPose,
        VecBuilder.fill(0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1));
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @param initialPose The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistance,
      double rightDistance,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DifferentialDriveOdometry(gyroAngle, leftDistance, rightDistance, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftPosition The distance traveled by the left encoder in meters.
   * @param rightPosition The distance traveled by the right encoder in meters.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, double leftPosition, double rightPosition, Pose2d pose) {
    resetPosition(
        gyroAngle, new DifferentialDriveWheelPositions(leftPosition, rightPosition), pose);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param distanceLeft The total distance travelled by the left wheel in meters.
   * @param distanceRight The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(Rotation2d gyroAngle, double distanceLeft, double distanceRight) {
    return update(gyroAngle, new DifferentialDriveWheelPositions(distanceLeft, distanceRight));
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTime Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param distanceLeft The total distance travelled by the left wheel in meters.
   * @param distanceRight The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTime, Rotation2d gyroAngle, double distanceLeft, double distanceRight) {
    return updateWithTime(
        currentTime, gyroAngle, new DifferentialDriveWheelPositions(distanceLeft, distanceRight));
  }
}
