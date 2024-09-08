// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <optional>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/Kinematics.h"
#include "frc/kinematics/Odometry.h"
#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {
/**
 * This class wraps odometry to fuse latency-compensated
 * vision measurements with encoder measurements. Robot code should not use this
 * directly- Instead, use the particular type for your drivetrain (e.g.,
 * DifferentialDrivePoseEstimator). It will correct for noisy vision
 * measurements and encoder drift. It is intended to be an easy drop-in for
 * Odometry.
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 *
 * @tparam WheelSpeeds Wheel speeds type.
 * @tparam WheelPositions Wheel positions type.
 */
template <typename WheelSpeeds, typename WheelPositions>
class WPILIB_DLLEXPORT PoseEstimator {
 public:
  /**
   * Constructs a PoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param odometry A correctly-configured odometry object for your drivetrain.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  PoseEstimator(Kinematics<WheelSpeeds, WheelPositions>& kinematics,
                Odometry<WheelSpeeds, WheelPositions>& odometry,
                const wpi::array<double, 3>& stateStdDevs,
                const wpi::array<double, 3>& visionMeasurementStdDevs);

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
      const wpi::array<double, 3>& visionMeasurementStdDevs);

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances traveled by the encoders.
   * @param pose The estimated pose of the robot on the field.
   */
  void ResetPosition(const Rotation2d& gyroAngle,
                     const WheelPositions& wheelPositions, const Pose2d& pose);

  /**
   * Resets the robot's pose.
   *
   * @param pose The pose to reset to.
   */
  void ResetPose(const Pose2d& pose);

  /**
   * Resets the robot's translation.
   *
   * @param translation The pose to translation to.
   */
  void ResetTranslation(const Translation2d& translation);

  /**
   * Resets the robot's rotation.
   *
   * @param rotation The rotation to reset to.
   */
  void ResetRotation(const Rotation2d& rotation);

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Return the pose at a given timestamp, if the buffer is not empty.
   *
   * @param timestamp The pose's timestamp.
   * @return The pose at the given timestamp (or std::nullopt if the buffer is
   * empty).
   */
  std::optional<Pose2d> SampleAt(units::second_t timestamp) const;

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
   * @param visionRobotPose The pose of the robot as measured by the vision
   *     camera.
   * @param timestamp The timestamp of the vision measurement in seconds. Note
   *     that if you don't use your own time source by calling UpdateWithTime(),
   *     then you must use a timestamp with an epoch since FPGA startup (i.e.,
   *     the epoch of this timestamp is the same epoch as
   *     frc::Timer::GetFPGATimestamp(). This means that you should use
   *     frc::Timer::GetFPGATimestamp() as your time source in this case.
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
   * @param gyroAngle      The current gyro angle.
   * @param wheelPositions The distances traveled by the encoders.
   *
   * @return The estimated pose of the robot in meters.
   */
  Pose2d Update(const Rotation2d& gyroAngle,
                const WheelPositions& wheelPositions);

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This
   * should be called every loop.
   *
   * @param currentTime   The time at which this method was called.
   * @param gyroAngle     The current gyro angle.
   * @param wheelPositions The distances traveled by the encoders.
   *
   * @return The estimated pose of the robot in meters.
   */
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        const WheelPositions& wheelPositions);

 private:
  /**
   * Removes stale vision updates that won't affect sampling.
   */
  void CleanUpVisionUpdates();

  struct VisionUpdate {
    // The vision-compensated pose estimate
    Pose2d visionPose;

    // The pose estimated based solely on odometry
    Pose2d odometryPose;

    /**
     * Returns the vision-compensated version of the pose. Specifically, changes
     * the pose from being relative to this record's odometry pose to being
     * relative to this record's vision pose.
     *
     * @param pose The pose to compensate.
     * @return The compensated pose.
     */
    Pose2d Compensate(const Pose2d& pose) const {
      auto delta = pose - odometryPose;
      return visionPose + delta;
    }
  };

  static constexpr units::second_t kBufferDuration = 1.5_s;

  Odometry<WheelSpeeds, WheelPositions>& m_odometry;
  wpi::array<double, 3> m_q{wpi::empty_array};
  Eigen::Matrix3d m_visionK = Eigen::Matrix3d::Zero();

  // Maps timestamps to odometry-only pose estimates
  TimeInterpolatableBuffer<Pose2d> m_odometryPoseBuffer{kBufferDuration};
  // Maps timestamps to vision updates
  // Always contains one entry before the oldest entry in m_odometryPoseBuffer,
  // unless there have been no vision measurements after the last reset
  std::map<units::second_t, VisionUpdate> m_visionUpdates;

  Pose2d m_poseEstimate;
};
}  // namespace frc

#include "frc/estimator/PoseEstimator.inc"
