// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/estimator/PoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveOdometry3d.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps Mecanum Drive Odometry to fuse latency-compensated
 * vision measurements with mecanum drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an
 * easy drop-in for MecanumDriveOdometry3d. It is also intended to be an easy
 * replacement for PoseEstimator, only requiring the addition of a standard
 * deviation for Z and appropriate conversions between 2D and 3D versions of
 * geometry classes. (See Pose3d(Pose2d), Rotation3d(Rotation2d),
 * Translation3d(Translation2d), and Pose3d.ToPose2d().)
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 */
class WPILIB_DLLEXPORT MecanumDrivePoseEstimator3d
    : public PoseEstimator3d<MecanumDriveWheelSpeeds,
                             MecanumDriveWheelPositions> {
 public:
  /**
   * Constructs a MecanumDrivePoseEstimator3d with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, 0.1 meters for z, and 0.1 radians for
   * angle. The default standard deviations of the vision measurements are 0.45
   * meters for x, 0.45 meters for y, 0.45 meters for z, and 0.45 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distance measured by each wheel.
   * @param initialPose The starting pose estimate.
   */
  MecanumDrivePoseEstimator3d(MecanumDriveKinematics& kinematics,
                              const Rotation3d& gyroAngle,
                              const MecanumDriveWheelPositions& wheelPositions,
                              const Pose3d& initialPose);

  /**
   * Constructs a MecanumDrivePoseEstimator3d.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distance measured by each wheel.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, z position in meters, and angle in
   * radians). Increase these numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, z position in
   * meters, and angle in radians). Increase these numbers to trust the vision
   * pose measurement less.
   */
  MecanumDrivePoseEstimator3d(
      MecanumDriveKinematics& kinematics, const Rotation3d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions,
      const Pose3d& initialPose, const wpi::array<double, 4>& stateStdDevs,
      const wpi::array<double, 4>& visionMeasurementStdDevs);

 private:
  MecanumDriveOdometry3d m_odometryImpl;
};

}  // namespace frc
