// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

/** WPIMath usage reporting IDs. */
public enum MathUsageId {
  /** DifferentialDriveKinematics. */
  kKinematics_DifferentialDrive,

  /** MecanumDriveKinematics. */
  kKinematics_MecanumDrive,

  /** SwerveDriveKinematics. */
  kKinematics_SwerveDrive,

  /** TrapezoidProfile. */
  kTrajectory_TrapezoidProfile,

  /** LinearFilter. */
  kFilter_Linear,

  /** DifferentialDriveOdometry. */
  kOdometry_DifferentialDrive,

  /** SwerveDriveOdometry. */
  kOdometry_SwerveDrive,

  /** MecanumDriveOdometry. */
  kOdometry_MecanumDrive,

  /** PIDController. */
  kController_PIDController2,

  /** ProfiledPIDController. */
  kController_ProfiledPIDController,

  /** BangBangController. */
  kController_BangBangController,

  /** PathWeaver Trajectory. */
  kTrajectory_PathWeaver,

  /** Linear Quadratic Regulator. */
  kController_LinearQuadraticRegulator,

  /** Kalman Filter. */
  kEstimator_KalmanFilter,

  /** Pose Estimator. */
  kEstimator_PoseEstimator,

  /** 3D Pose Estimator. */
  kEstimator_PoseEstimator3d,

  /** Linear System Loop. */
  kSystem_LinearSystemLoop,
}
