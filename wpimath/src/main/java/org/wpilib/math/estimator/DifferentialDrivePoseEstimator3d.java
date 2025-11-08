// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveOdometry3d;
import org.wpilib.math.kinematics.DifferentialDriveWheelPositions;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;

/**
 * This class wraps {@link DifferentialDriveOdometry3d Differential Drive Odometry} to fuse
 * latency-compensated vision measurements with differential drive encoder measurements. It is
 * intended to be a drop-in replacement for {@link DifferentialDriveOdometry3d}; in fact, if you
 * never call {@link DifferentialDrivePoseEstimator3d#addVisionMeasurement} and only call {@link
 * DifferentialDrivePoseEstimator3d#update} then this will behave exactly the same as
 * DifferentialDriveOdometry3d. It is also intended to be an easy replacement for {@link
 * DifferentialDrivePoseEstimator}, only requiring the addition of a standard deviation for Z and
 * appropriate conversions between 2D and 3D versions of geometry classes. (See {@link
 * Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link DifferentialDrivePoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link DifferentialDrivePoseEstimator3d#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it then this class will behave exactly like regular encoder odometry.
 */
public class DifferentialDrivePoseEstimator3d
    extends PoseEstimator3d<DifferentialDriveWheelPositions> {
  /**
   * Constructs a DifferentialDrivePoseEstimator3d with default standard deviations for the model
   * and vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.02 meters for x, 0.02 meters for
   * y, 0.02 meters for z, and 0.01 radians for angle. The default standard deviations of the vision
   * measurements are 0.1 meters for x, 0.1 meters for y, 0.1 meters for z, and 0.1 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param leftDistance The distance traveled by the left encoder in meters.
   * @param rightDistance The distance traveled by the right encoder in meters.
   * @param initialPose The starting pose estimate.
   */
  public DifferentialDrivePoseEstimator3d(
      DifferentialDriveKinematics kinematics,
      Rotation3d gyroAngle,
      double leftDistance,
      double rightDistance,
      Pose3d initialPose) {
    this(
        kinematics,
        gyroAngle,
        leftDistance,
        rightDistance,
        initialPose,
        VecBuilder.fill(0.02, 0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1));
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator3d.
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
  public DifferentialDrivePoseEstimator3d(
      DifferentialDriveKinematics kinematics,
      Rotation3d gyroAngle,
      double leftDistance,
      double rightDistance,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DifferentialDriveOdometry3d(gyroAngle, leftDistance, rightDistance, initialPose),
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
      Rotation3d gyroAngle, double leftPosition, double rightPosition, Pose3d pose) {
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
  public Pose3d update(Rotation3d gyroAngle, double distanceLeft, double distanceRight) {
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
  public Pose3d updateWithTime(
      double currentTime, Rotation3d gyroAngle, double distanceLeft, double distanceRight) {
    return updateWithTime(
        currentTime, gyroAngle, new DifferentialDriveWheelPositions(distanceLeft, distanceRight));
  }
}
