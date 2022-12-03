// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveOdometry.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps Mecanum Drive Odometry to fuse latency-compensated
 * vision measurements with mecanum drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an
 * easy drop-in for MecanumDriveOdometry.
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 *
 * The state-space system used internally has the following states (x) and
 * outputs (y):
 *
 * <strong> x = [x, y, theta]ᵀ </strong> in the field
 * coordinate system containing x position, y position, and heading.
 *
 * <strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y
 * position, and heading.
 */
class WPILIB_DLLEXPORT MecanumDrivePoseEstimator {
 public:
  /**
   * Constructs a MecanumDrivePoseEstimator with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.45 meters for x, 0.45 meters for y, and 0.45 radians for heading.
   *
   * @param kinematics               A correctly-configured kinematics object
   *                                 for your drivetrain.
   * @param gyroAngle                The current gyro angle.
   * @param wheelPositions           The distance measured by each wheel.
   * @param initialPose              The starting pose estimate.
   */
  MecanumDrivePoseEstimator(MecanumDriveKinematics& kinematics,
                            const Rotation2d& gyroAngle,
                            const MecanumDriveWheelPositions& wheelPositions,
                            const Pose2d& initialPose);

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param kinematics               A correctly-configured kinematics object
   *                                 for your drivetrain.
   * @param gyroAngle                The current gyro angle.
   * @param wheelPositions           The distance measured by each wheel.
   * @param initialPose              The starting pose estimate.

   * @param stateStdDevs             Standard deviations of model states.
   *                                 Increase these numbers to trust your
   *                                 model's state estimates less. This matrix
   *                                 is in the form [x, y, theta]ᵀ, with units
   *                                 in meters and radians.
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]ᵀ, with units in meters and
   *                                 radians.
   */
  MecanumDrivePoseEstimator(
      MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions,
      const Pose2d& initialPose, const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs);

  /**
   * Sets the pose estimator's trust of global measurements. This might be used
   * to change trust in vision measurements after the autonomous period, or to
   * change trust as distance to a vision target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]ᵀ, with units in meters and
   *                                 radians.
   */
  void SetVisionMeasurementStdDevs(
      const wpi::array<double, 3>& visionMeasurementStdDevs);

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances measured at each wheel.
   * @param pose      The position on the field that your robot is at.
   */
  void ResetPosition(const Rotation2d& gyroAngle,
                     const MecanumDriveWheelPositions& wheelPositions,
                     const Pose2d& pose);

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Add a vision measurement to the Kalman Filter. This will correct
   * the odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
   *
   * To promote stability of the pose estimate and make it robust to bad vision
   * data, we recommend only adding vision measurements that are already within
   * one meter or so of the current pose estimate.
   *
   * @param visionRobotPose The pose of the robot as measured by the vision
   *                        camera.
   * @param timestamp       The timestamp of the vision measurement in seconds.
   *                        Note that if you don't use your own time source by
   *                        calling UpdateWithTime() then you must use a
   *                        timestamp with an epoch since FPGA startup
   *                        (i.e. the epoch of this timestamp is the same
   *                        epoch as Timer#GetFPGATimestamp.) This means
   *                        that you should use Timer#GetFPGATimestamp as your
   *                        time source or sync the epochs.
   */
  void AddVisionMeasurement(const Pose2d& visionRobotPose,
                            units::second_t timestamp);

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct
   * the odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
   *
   * To promote stability of the pose estimate and make it robust to bad vision
   * data, we recommend only adding vision measurements that are already within
   * one meter or so of the current pose estimate.
   *
   * Note that the vision measurement standard deviations passed into this
   * method will continue to apply to future measurements until a subsequent
   * call to SetVisionMeasurementStdDevs() or this method.
   *
   * @param visionRobotPose          The pose of the robot as measured by the
   *                                 vision camera.
   * @param timestamp                The timestamp of the vision measurement in
   *                                 seconds. Note that if you don't use your
   *                                 own time source by calling
   *                                 UpdateWithTime(), then you must use a
   *                                 timestamp with an epoch since FPGA startup
   *                                 (i.e. the epoch of this timestamp is the
   *                                 same epoch as
   *                                 frc::Timer::GetFPGATimestamp(). This means
   *                                 that you should use
   *                                 frc::Timer::GetFPGATimestamp() as your
   *                                 time source in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]ᵀ, with units in meters and
   *                                 radians.
   */
  void AddVisionMeasurement(
      const Pose2d& visionRobotPose, units::second_t timestamp,
      const wpi::array<double, 3>& visionMeasurementStdDevs) {
    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
    AddVisionMeasurement(visionRobotPose, timestamp);
  }

  /**
   * Updates the the Kalman Filter using only wheel encoder
   * information. This should be called every loop.
   *
   * @param gyroAngle   The current gyro angle.
   * @param wheelPositions The distances measured at each wheel.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d Update(const Rotation2d& gyroAngle,
                const MecanumDriveWheelPositions& wheelPositions);

  /**
   * Updates the the Kalman Filter using only wheel encoder
   * information. This should be called every loop.
   *
   * @param currentTime Time at which this method was called, in seconds.
   * @param gyroAngle   The current gyroscope angle.
   * @param wheelPositions The distances measured at each wheel.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        const MecanumDriveWheelPositions& wheelPositions);

 private:
  struct InterpolationRecord {
    // The pose observed given the current sensor inputs and the previous pose.
    Pose2d pose;

    // The current gyroscope angle.
    Rotation2d gyroAngle;

    // The distances measured at each wheel.
    MecanumDriveWheelPositions wheelPositions;

    /**
     * Checks equality between this InterpolationRecord and another object.
     *
     * @param other The other object.
     * @return Whether the two objects are equal.
     */
    bool operator==(const InterpolationRecord& other) const = default;

    /**
     * Checks inequality between this InterpolationRecord and another object.
     *
     * @param other The other object.
     * @return Whether the two objects are not equal.
     */
    bool operator!=(const InterpolationRecord& other) const = default;

    /**
     * Interpolates between two InterpolationRecords.
     *
     * @param endValue The end value for the interpolation.
     * @param i The interpolant (fraction).
     *
     * @return The interpolated state.
     */
    InterpolationRecord Interpolate(MecanumDriveKinematics& kinematics,
                                    InterpolationRecord endValue,
                                    double i) const;
  };

  MecanumDriveKinematics& m_kinematics;
  MecanumDriveOdometry m_odometry;
  wpi::array<double, 3> m_q{wpi::empty_array};
  Eigen::Matrix3d m_visionK = Eigen::Matrix3d::Zero();

  TimeInterpolatableBuffer<InterpolationRecord> m_poseBuffer{
      1.5_s, [this](const InterpolationRecord& start,
                    const InterpolationRecord& end, double t) {
        return start.Interpolate(this->m_kinematics, end, t);
      }};
};

}  // namespace frc
