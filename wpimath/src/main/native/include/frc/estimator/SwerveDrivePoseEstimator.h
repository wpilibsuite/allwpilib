// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include <fmt/format.h>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>
#include <wpi/timestamp.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry.h"
#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {

/**
 * This class wraps Swerve Drive Odometry to fuse latency-compensated
 * vision measurements with swerve drive encoder distance measurements. It is
 * intended to be a drop-in for SwerveDriveOdometry.
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave as regular encoder
 * odometry.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator {
 public:
  /**
   * Constructs a SwerveDrivePoseEstimator with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *     the swerve modules.
   * @param initialPose The starting pose estimate.
   */
  SwerveDrivePoseEstimator(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose)
      : SwerveDrivePoseEstimator{kinematics,      gyroAngle,
                                 modulePositions, initialPose,
                                 {0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}} {}

  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *     the swerve modules.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  SwerveDrivePoseEstimator(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose, const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs)
      : m_kinematics{kinematics},
        m_odometry{kinematics, gyroAngle, modulePositions, initialPose} {
    for (size_t i = 0; i < 3; ++i) {
      m_q[i] = stateStdDevs[i] * stateStdDevs[i];
    }

    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle       The angle reported by the gyroscope.
   * @param modulePositions The current distance and rotation measurements of
   *                        the swerve modules.
   * @param pose            The position on the field that your robot is at.
   */
  void ResetPosition(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& pose) {
    // Reset state estimate and error covariance
    m_odometry.ResetPosition(gyroAngle, modulePositions, pose);
    m_poseBuffer.Clear();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const { return m_odometry.GetPose(); }

  /**
   * Sets the pose estimator's trust in vision measurements. This might be used
   * to change trust in vision measurements after the autonomous period, or to
   * change trust as distance to a vision target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  void SetVisionMeasurementStdDevs(
      const wpi::array<double, 3>& visionMeasurementStdDevs) {
    wpi::array<double, 3> r{wpi::empty_array};
    for (size_t i = 0; i < 3; ++i) {
      r[i] = visionMeasurementStdDevs[i] * visionMeasurementStdDevs[i];
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (size_t row = 0; row < 3; ++row) {
      if (m_q[row] == 0.0) {
        m_visionK(row, row) = 0.0;
      } else {
        m_visionK(row, row) =
            m_q[row] / (m_q[row] + std::sqrt(m_q[row] * r[row]));
      }
    }
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the
   * odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
   *
   * To promote stability of the pose estimate and make it robust to bad vision
   * data, we recommend only adding vision measurements that are already within
   * one meter or so of the current pose estimate.
   *
   * @param visionRobotPose The pose of the robot as measured by the vision
   *    camera.
   * @param timestamp The timestamp of the vision measurement in seconds. Note
   *     that if you don't use your own time source by calling UpdateWithTime()
   *     then you must use a timestamp with an epoch since FPGA startup (i.e.,
   *     the epoch of this timestamp is the same epoch as
   *     frc::Timer::GetFPGATimestamp().) This means that you should use
   *     frc::Timer::GetFPGATimestamp() as your time source or sync the epochs.
   */
  void AddVisionMeasurement(const Pose2d& visionRobotPose,
                            units::second_t timestamp) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's
    // timespan, skip.
    if (!m_poseBuffer.GetInternalBuffer().empty() &&
        m_poseBuffer.GetInternalBuffer().front().first - kBufferDuration >
            timestamp) {
      return;
    }

    // Step 1: Get the estimated pose from when the vision measurement was made.
    auto sample = m_poseBuffer.Sample(timestamp);

    if (!sample.has_value()) {
      return;
    }

    // Step 2: Measure the twist between the odometry pose and the vision pose
    auto twist = sample.value().pose.Log(visionRobotPose);

    // Step 3: We should not trust the twist entirely, so instead we scale this
    // twist by a Kalman gain matrix representing how much we trust vision
    // measurements compared to our current pose.
    frc::Vectord<3> k_times_twist =
        m_visionK * frc::Vectord<3>{twist.dx.value(), twist.dy.value(),
                                    twist.dtheta.value()};

    // Step 4: Convert back to Twist2d
    Twist2d scaledTwist{units::meter_t{k_times_twist(0)},
                        units::meter_t{k_times_twist(1)},
                        units::radian_t{k_times_twist(2)}};

    // Step 5: Reset Odometry to state at sample with vision adjustment.
    m_odometry.ResetPosition(sample.value().gyroAngle,
                             sample.value().modulePositions,
                             sample.value().pose.Exp(scaledTwist));

    // Step 6: Record the current pose to allow multiple measurements from the
    // same timestamp
    m_poseBuffer.AddSample(timestamp,
                           {GetEstimatedPosition(), sample.value().gyroAngle,
                            sample.value().modulePositions});

    // Step 7: Replay odometry inputs between sample time and latest recorded
    // sample to update the pose buffer and correct odometry.
    auto internal_buf = m_poseBuffer.GetInternalBuffer();

    auto upper_bound = std::lower_bound(
        internal_buf.begin(), internal_buf.end(), timestamp,
        [](const auto& pair, auto t) { return t > pair.first; });

    for (auto entry = upper_bound; entry != internal_buf.end(); entry++) {
      UpdateWithTime(entry->first, entry->second.gyroAngle,
                     entry->second.modulePositions);
    }
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the
   * odometry pose estimate while still accounting for measurement noise.
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
   * @param visionRobotPose The pose of the robot as measured by the vision
   *     camera.
   * @param timestamp The timestamp of the vision measurement in seconds. Note
   *     that if you don't use your own time source by calling UpdateWithTime(),
   *     then you must use a timestamp with an epoch since FPGA startup (i.e.,
   *     the epoch of this timestamp is the same epoch as
   *     frc::Timer::GetFPGATimestamp(). This means that you should use
   *     frc::Timer::GetFPGATimestamp() as your time source in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  void AddVisionMeasurement(
      const Pose2d& visionRobotPose, units::second_t timestamp,
      const wpi::array<double, 3>& visionMeasurementStdDevs) {
    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
    AddVisionMeasurement(visionRobotPose, timestamp);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This
   * should be called every loop.
   *
   * @param gyroAngle       The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *                        the swerve modules.
   * @return The estimated robot pose in meters.
   */
  Pose2d Update(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                          modulePositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This
   * should be called every loop.
   *
   * @param currentTime     Time at which this method was called, in seconds.
   * @param gyroAngle       The current gyro angle.
   * @param modulePositions The current distance traveled and rotations of
   *                        the swerve modules.
   * @return The estimated robot pose in meters.
   */
  Pose2d UpdateWithTime(
      units::second_t currentTime, const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    m_odometry.Update(gyroAngle, modulePositions);

    wpi::array<SwerveModulePosition, NumModules> internalModulePositions{
        wpi::empty_array};

    for (size_t i = 0; i < NumModules; i++) {
      internalModulePositions[i].distance = modulePositions[i].distance;
      internalModulePositions[i].angle = modulePositions[i].angle;
    }

    m_poseBuffer.AddSample(currentTime, {GetEstimatedPosition(), gyroAngle,
                                         internalModulePositions});

    return GetEstimatedPosition();
  }

 private:
  struct InterpolationRecord {
    // The pose observed given the current sensor inputs and the previous pose.
    Pose2d pose;

    // The current gyroscope angle.
    Rotation2d gyroAngle;

    // The distances traveled and rotations meaured at each module.
    wpi::array<SwerveModulePosition, NumModules> modulePositions;

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
    InterpolationRecord Interpolate(
        SwerveDriveKinematics<NumModules>& kinematics,
        InterpolationRecord endValue, double i) const {
      if (i < 0) {
        return *this;
      } else if (i > 1) {
        return endValue;
      } else {
        // Find the new module distances.
        wpi::array<SwerveModulePosition, NumModules> modulePositions{
            wpi::empty_array};
        // Find the distance between this measurement and the
        // interpolated measurement.
        wpi::array<SwerveModulePosition, NumModules> modulesDelta{
            wpi::empty_array};

        for (size_t i = 0; i < NumModules; i++) {
          modulePositions[i].distance =
              wpi::Lerp(this->modulePositions[i].distance,
                        endValue.modulePositions[i].distance, i);
          modulePositions[i].angle =
              wpi::Lerp(this->modulePositions[i].angle,
                        endValue.modulePositions[i].angle, i);

          modulesDelta[i].distance =
              modulePositions[i].distance - this->modulePositions[i].distance;
          modulesDelta[i].angle = modulePositions[i].angle;
        }

        // Find the new gyro angle.
        auto gyro = wpi::Lerp(this->gyroAngle, endValue.gyroAngle, i);

        // Create a twist to represent this changed based on the interpolated
        // sensor inputs.
        auto twist = kinematics.ToTwist2d(modulesDelta);
        twist.dtheta = (gyro - gyroAngle).Radians();

        return {pose.Exp(twist), gyro, modulePositions};
      }
    }
  };

  static constexpr units::second_t kBufferDuration = 1.5_s;

  SwerveDriveKinematics<NumModules>& m_kinematics;
  SwerveDriveOdometry<NumModules> m_odometry;
  wpi::array<double, 3> m_q{wpi::empty_array};
  Eigen::Matrix3d m_visionK = Eigen::Matrix3d::Zero();

  TimeInterpolatableBuffer<InterpolationRecord> m_poseBuffer{
      kBufferDuration, [this](const InterpolationRecord& start,
                              const InterpolationRecord& end, double t) {
        return start.Interpolate(this->m_kinematics, end, t);
      }};
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDrivePoseEstimator<4>;

}  // namespace frc
