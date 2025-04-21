// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/estimator/PoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry3d.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps Differential Drive Odometry to fuse latency-compensated
 * vision measurements with differential drive encoder measurements. It will
 * correct for noisy vision measurements and encoder drift. It is intended to be
 * an easy drop-in for DifferentialDriveOdometry3d. In fact, if you never call
 * AddVisionMeasurement(), and only call Update(), this will behave exactly the
 * same as DifferentialDriveOdometry3d. It is also intended to be an easy
 * replacement for PoseEstimator, only requiring the addition of a standard
 * deviation for Z and appropriate conversions between 2D and 3D versions of
 * geometry classes. (See Pose3d(Pose2d), Rotation3d(Rotation2d),
 * Translation3d(Translation2d), and Pose3d.ToPose2d().)
 *
 * Update() should be called every robot loop (if your robot loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time via the constructor).
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 */
class WPILIB_DLLEXPORT DifferentialDrivePoseEstimator3d
    : public PoseEstimator3d<DifferentialDriveWheelSpeeds,
                             DifferentialDriveWheelPositions> {
 public:
  /**
   * Constructs a DifferentialDrivePoseEstimator3d with default standard
   * deviations for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.02 meters for x, 0.02 meters for y, 0.02 meters for z, and 0.01 radians
   * for angle. The default standard deviations of the vision measurements are
   * 0.1 meters for x, 0.1 meters for y, 0.1 meters for z, and 0.1 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The estimated initial pose.
   */
  DifferentialDrivePoseEstimator3d(DifferentialDriveKinematics& kinematics,
                                   const Rotation3d& gyroAngle,
                                   units::meter_t leftDistance,
                                   units::meter_t rightDistance,
                                   const Pose3d& initialPose);

  /**
   * Constructs a DifferentialDrivePoseEstimator3d.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, z position in meters, and angle in
   * radians). Increase these numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, z position in
   * meters, and angle in radians). Increase these numbers to trust the vision
   * pose measurement less.
   */
  DifferentialDrivePoseEstimator3d(
      DifferentialDriveKinematics& kinematics, const Rotation3d& gyroAngle,
      units::meter_t leftDistance, units::meter_t rightDistance,
      const Pose3d& initialPose, const wpi::array<double, 4>& stateStdDevs,
      const wpi::array<double, 4>& visionMeasurementStdDevs);

  /**
   * Resets the robot's position on the field.
   *
   * @param gyroAngle The current gyro angle.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param pose The estimated pose of the robot on the field.
   */
  void ResetPosition(const Rotation3d& gyroAngle, units::meter_t leftDistance,
                     units::meter_t rightDistance, const Pose3d& pose) {
    PoseEstimator3d::ResetPosition(gyroAngle, {leftDistance, rightDistance},
                                   pose);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This
   * should be called every loop.
   *
   * @param gyroAngle     The current gyro angle.
   * @param leftDistance  The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   *
   * @return The estimated pose of the robot.
   */
  Pose3d Update(const Rotation3d& gyroAngle, units::meter_t leftDistance,
                units::meter_t rightDistance) {
    return PoseEstimator3d::Update(gyroAngle, {leftDistance, rightDistance});
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This
   * should be called every loop.
   *
   * @param currentTime   The time at which this method was called.
   * @param gyroAngle     The current gyro angle.
   * @param leftDistance  The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   *
   * @return The estimated pose of the robot.
   */
  Pose3d UpdateWithTime(units::second_t currentTime,
                        const Rotation3d& gyroAngle,
                        units::meter_t leftDistance,
                        units::meter_t rightDistance) {
    return PoseEstimator3d::UpdateWithTime(currentTime, gyroAngle,
                                           {leftDistance, rightDistance});
  }

 private:
  DifferentialDriveOdometry3d m_odometryImpl;
};

}  // namespace frc
