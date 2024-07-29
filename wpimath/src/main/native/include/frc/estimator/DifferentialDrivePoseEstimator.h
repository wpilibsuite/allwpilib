// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/estimator/PoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps Differential Drive Odometry to fuse latency-compensated
 * vision measurements with differential drive encoder measurements. It will
 * correct for noisy vision measurements and encoder drift. It is intended to be
 * an easy drop-in for DifferentialDriveOdometry. In fact, if you never call
 * AddVisionMeasurement(), and only call Update(), this will behave exactly the
 * same as DifferentialDriveOdometry.
 *
 * Update() should be called every robot loop (if your robot loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time via the constructor).
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 */
class WPILIB_DLLEXPORT DifferentialDrivePoseEstimator
    : public PoseEstimator<DifferentialDriveWheelSpeeds,
                           DifferentialDriveWheelPositions> {
 public:
  /**
   * Constructs a DifferentialDrivePoseEstimator with default standard
   * deviations for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.02 meters for x, 0.02 meters for y, and 0.01 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The estimated initial pose.
   */
  DifferentialDrivePoseEstimator(DifferentialDriveKinematics& kinematics,
                                 const Rotation2d& gyroAngle,
                                 units::meter_t leftDistance,
                                 units::meter_t rightDistance,
                                 const Pose2d& initialPose);

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics& kinematics, const Rotation2d& gyroAngle,
      units::meter_t leftDistance, units::meter_t rightDistance,
      const Pose2d& initialPose, const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs);

  /**
   * Resets the robot's position on the field.
   *
   * @param gyroAngle The current gyro angle.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param pose The estimated pose of the robot on the field.
   */
  void ResetPosition(const Rotation2d& gyroAngle, units::meter_t leftDistance,
                     units::meter_t rightDistance, const Pose2d& pose) {
    PoseEstimator<DifferentialDriveWheelSpeeds,
                  DifferentialDriveWheelPositions>::
        ResetPosition(gyroAngle, {leftDistance, rightDistance}, pose);
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
  Pose2d Update(const Rotation2d& gyroAngle, units::meter_t leftDistance,
                units::meter_t rightDistance) {
    return PoseEstimator<
        DifferentialDriveWheelSpeeds,
        DifferentialDriveWheelPositions>::Update(gyroAngle,
                                                 {leftDistance, rightDistance});
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
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        units::meter_t leftDistance,
                        units::meter_t rightDistance) {
    return PoseEstimator<
        DifferentialDriveWheelSpeeds,
        DifferentialDriveWheelPositions>::UpdateWithTime(currentTime, gyroAngle,
                                                         {leftDistance,
                                                          rightDistance});
  }

 private:
  DifferentialDriveOdometry m_odometryImpl;
};

}  // namespace frc
