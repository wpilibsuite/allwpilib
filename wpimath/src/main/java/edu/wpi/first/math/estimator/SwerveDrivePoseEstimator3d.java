// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveOdometry3d;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N4;

/**
 * This class wraps {@link SwerveDriveOdometry3d Swerve Drive Odometry} to fuse latency-compensated
 * vision measurements with swerve drive encoder distance measurements. It is intended to be a
 * drop-in replacement for {@link SwerveDriveOdometry3d}. It is also intended to be an easy
 * replacement for {@link SwerveDrivePoseEstimator}, only requiring the addition of a standard
 * deviation for Z and appropriate conversions between 2D and 3D versions of geometry classes. (See
 * {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link SwerveDrivePoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link SwerveDrivePoseEstimator3d#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave as regular encoder odometry.
 */
public class SwerveDrivePoseEstimator3d extends PoseEstimator3d<SwerveModulePosition[]> {
  private final int m_numModules;

  /**
   * Constructs a SwerveDrivePoseEstimator3d with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * 0.1 meters for z, and 0.1 radians for angle. The default standard deviations of the vision
   * measurements are 0.9 meters for x, 0.9 meters for y, 0.9 meters for z, and 0.9 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @param initialPoseMeters The starting pose estimate.
   */
  public SwerveDrivePoseEstimator3d(
      SwerveDriveKinematics kinematics,
      Rotation3d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose3d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        modulePositions,
        initialPoseMeters,
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9, 0.9));
  }

  /**
   * Constructs a SwerveDrivePoseEstimator3d.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of the swerve modules.
   * @param initialPoseMeters The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public SwerveDrivePoseEstimator3d(
      SwerveDriveKinematics kinematics,
      Rotation3d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose3d initialPoseMeters,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new SwerveDriveOdometry3d(kinematics, gyroAngle, modulePositions, initialPoseMeters),
        stateStdDevs,
        visionMeasurementStdDevs);

    m_numModules = modulePositions.length;
  }

  @Override
  public Pose3d updateWithTime(
      double currentTimeSeconds, Rotation3d gyroAngle, SwerveModulePosition[] wheelPositions) {
    if (wheelPositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }

    return super.updateWithTime(currentTimeSeconds, gyroAngle, wheelPositions);
  }
}
