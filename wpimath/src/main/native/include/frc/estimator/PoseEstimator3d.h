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

#include "frc/EigenCore.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/Kinematics.h"
#include "frc/kinematics/Odometry3d.h"
#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {

/**
 * This class wraps odometry to fuse latency-compensated
 * vision measurements with encoder measurements. Robot code should not use this
 * directly- Instead, use the particular type for your drivetrain (e.g.,
 * DifferentialDrivePoseEstimator3d). It will correct for noisy vision
 * measurements and encoder drift. It is intended to be an easy drop-in for
 * Odometry3d. It is also intended to be an easy replacement for PoseEstimator,
 * only requiring the addition of a standard deviation for Z and appropriate
 * conversions between 2D and 3D versions of geometry classes. (See
 * Pose3d(Pose2d), Rotation3d(Rotation2d), Translation3d(Translation2d), and
 * Pose3d.ToPose2d().)
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
class WPILIB_DLLEXPORT PoseEstimator3d {
 public:
  /**
   * Constructs a PoseEstimator3d.
   *
   * @warning The initial pose estimate will always be the default pose,
   * regardless of the odometry's current pose.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param odometry A correctly-configured odometry object for your drivetrain.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and z position
   * in meters, and angle in radians). Increase these numbers to trust the
   * vision pose measurement less.
   */
  PoseEstimator3d(Kinematics<WheelSpeeds, WheelPositions>& kinematics,
                  Odometry3d<WheelSpeeds, WheelPositions>& odometry,
                  const wpi::array<double, 4>& stateStdDevs,
                  const wpi::array<double, 4>& visionMeasurementStdDevs)
      : m_odometry(odometry) {
    for (size_t i = 0; i < 4; ++i) {
      m_q[i] = stateStdDevs[i] * stateStdDevs[i];
    }

    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

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
      const wpi::array<double, 4>& visionMeasurementStdDevs) {
    wpi::array<double, 4> r{wpi::empty_array};
    for (size_t i = 0; i < 4; ++i) {
      r[i] = visionMeasurementStdDevs[i] * visionMeasurementStdDevs[i];
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (size_t row = 0; row < 4; ++row) {
      if (m_q[row] == 0.0) {
        m_visionK(row, row) = 0.0;
      } else {
        m_visionK(row, row) =
            m_q[row] / (m_q[row] + std::sqrt(m_q[row] * r[row]));
      }
    }
    double angle_gain = m_visionK(3, 3);
    m_visionK(4, 4) = angle_gain;
    m_visionK(5, 5) = angle_gain;
  }

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
  void ResetPosition(const Rotation3d& gyroAngle,
                     const WheelPositions& wheelPositions, const Pose3d& pose) {
    // Reset state estimate and error covariance
    m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
    m_odometryPoseBuffer.Clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.GetPose();
  }

  /**
   * Resets the robot's pose.
   *
   * @param pose The pose to reset to.
   */
  void ResetPose(const Pose3d& pose) {
    m_odometry.ResetPose(pose);
    m_odometryPoseBuffer.Clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.GetPose();
  }

  /**
   * Resets the robot's translation.
   *
   * @param translation The pose to translation to.
   */
  void ResetTranslation(const Translation3d& translation) {
    m_odometry.ResetTranslation(translation);
    m_odometryPoseBuffer.Clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.GetPose();
  }

  /**
   * Resets the robot's rotation.
   *
   * @param rotation The rotation to reset to.
   */
  void ResetRotation(const Rotation3d& rotation) {
    m_odometry.ResetRotation(rotation);
    m_odometryPoseBuffer.Clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.GetPose();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  Pose3d GetEstimatedPosition() const { return m_poseEstimate; }

  /**
   * Return the pose at a given timestamp, if the buffer is not empty.
   *
   * @param timestamp The pose's timestamp.
   * @return The pose at the given timestamp (or std::nullopt if the buffer is
   * empty).
   */
  std::optional<Pose3d> SampleAt(units::second_t timestamp) const {
    // Step 0: If there are no odometry updates to sample, skip.
    if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
      return std::nullopt;
    }

    // Step 1: Make sure timestamp matches the sample from the odometry pose
    // buffer. (When sampling, the buffer will always use a timestamp
    // between the first and last timestamps)
    units::second_t oldestOdometryTimestamp =
        m_odometryPoseBuffer.GetInternalBuffer().front().first;
    units::second_t newestOdometryTimestamp =
        m_odometryPoseBuffer.GetInternalBuffer().back().first;
    timestamp =
        std::clamp(timestamp, oldestOdometryTimestamp, newestOdometryTimestamp);

    // Step 2: If there are no applicable vision updates, use the odometry-only
    // information.
    if (m_visionUpdates.empty() || timestamp < m_visionUpdates.begin()->first) {
      return m_odometryPoseBuffer.Sample(timestamp);
    }

    // Step 3: Get the latest vision update from before or at the timestamp to
    // sample at.
    // First, find the iterator past the sample timestamp, then go back one.
    // Note that upper_bound() won't return begin() because we check begin()
    // earlier.
    auto floorIter = m_visionUpdates.upper_bound(timestamp);
    --floorIter;
    auto visionUpdate = floorIter->second;

    // Step 4: Get the pose measured by odometry at the time of the sample.
    auto odometryEstimate = m_odometryPoseBuffer.Sample(timestamp);

    // Step 5: Apply the vision compensation to the odometry pose.
    // TODO Replace with std::optional::transform() in C++23
    if (odometryEstimate) {
      return visionUpdate.Compensate(*odometryEstimate);
    }
    return std::nullopt;
  }

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
  void AddVisionMeasurement(const Pose3d& visionRobotPose,
                            units::second_t timestamp) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's
    // timespan, skip.
    if (m_odometryPoseBuffer.GetInternalBuffer().empty() ||
        m_odometryPoseBuffer.GetInternalBuffer().front().first -
                kBufferDuration >
            timestamp) {
      return;
    }

    // Step 1: Clean up any old entries
    CleanUpVisionUpdates();

    // Step 2: Get the pose measured by odometry at the moment the vision
    // measurement was made.
    auto odometrySample = m_odometryPoseBuffer.Sample(timestamp);

    if (!odometrySample) {
      return;
    }

    // Step 3: Get the vision-compensated pose estimate at the moment the vision
    // measurement was made.
    auto visionSample = SampleAt(timestamp);

    if (!visionSample) {
      return;
    }

    // Step 4: Measure the twist between the old pose estimate and the vision
    // pose.
    auto twist = visionSample.value().Log(visionRobotPose);

    // Step 5: We should not trust the twist entirely, so instead we scale this
    // twist by a Kalman gain matrix representing how much we trust vision
    // measurements compared to our current pose.
    frc::Vectord<6> k_times_twist =
        m_visionK * frc::Vectord<6>{twist.dx.value(), twist.dy.value(),
                                    twist.dz.value(), twist.rx.value(),
                                    twist.ry.value(), twist.rz.value()};

    // Step 6: Convert back to Twist3d.
    Twist3d scaledTwist{
        units::meter_t{k_times_twist(0)},  units::meter_t{k_times_twist(1)},
        units::meter_t{k_times_twist(2)},  units::radian_t{k_times_twist(3)},
        units::radian_t{k_times_twist(4)}, units::radian_t{k_times_twist(5)}};

    // Step 7: Calculate and record the vision update.
    VisionUpdate visionUpdate{visionSample->Exp(scaledTwist), *odometrySample};
    m_visionUpdates[timestamp] = visionUpdate;

    // Step 8: Remove later vision measurements. (Matches previous behavior)
    auto firstAfter = m_visionUpdates.upper_bound(timestamp);
    m_visionUpdates.erase(firstAfter, m_visionUpdates.end());

    // Step 9: Update latest pose estimate. Since we cleared all updates after
    // this vision update, it's guaranteed to be the latest vision update.
    m_poseEstimate = visionUpdate.Compensate(m_odometry.GetPose());
  }

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
      const Pose3d& visionRobotPose, units::second_t timestamp,
      const wpi::array<double, 4>& visionMeasurementStdDevs) {
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
  Pose3d Update(const Rotation3d& gyroAngle,
                const WheelPositions& wheelPositions) {
    return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                          wheelPositions);
  }

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
  Pose3d UpdateWithTime(units::second_t currentTime,
                        const Rotation3d& gyroAngle,
                        const WheelPositions& wheelPositions) {
    auto odometryEstimate = m_odometry.Update(gyroAngle, wheelPositions);

    m_odometryPoseBuffer.AddSample(currentTime, odometryEstimate);

    if (m_visionUpdates.empty()) {
      m_poseEstimate = odometryEstimate;
    } else {
      auto visionUpdate = m_visionUpdates.rbegin()->second;
      m_poseEstimate = visionUpdate.Compensate(odometryEstimate);
    }

    return GetEstimatedPosition();
  }

 private:
  /**
   * Removes stale vision updates that won't affect sampling.
   */
  void CleanUpVisionUpdates() {
    // Step 0: If there are no odometry samples, skip.
    if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
      return;
    }

    // Step 1: Find the oldest timestamp that needs a vision update.
    units::second_t oldestOdometryTimestamp =
        m_odometryPoseBuffer.GetInternalBuffer().front().first;

    // Step 2: If there are no vision updates before that timestamp, skip.
    if (m_visionUpdates.empty() ||
        oldestOdometryTimestamp < m_visionUpdates.begin()->first) {
      return;
    }

    // Step 3: Find the newest vision update timestamp before or at the oldest
    // timestamp.
    // First, find the iterator past the oldest odometry timestamp, then go
    // back one. Note that upper_bound() won't return begin() because we check
    // begin() earlier.
    auto newestNeededVisionUpdate =
        m_visionUpdates.upper_bound(oldestOdometryTimestamp);
    --newestNeededVisionUpdate;

    // Step 4: Remove all entries strictly before the newest timestamp we need.
    m_visionUpdates.erase(m_visionUpdates.begin(), newestNeededVisionUpdate);
  }

  struct VisionUpdate {
    // The vision-compensated pose estimate
    Pose3d visionPose;

    // The pose estimated based solely on odometry
    Pose3d odometryPose;

    /**
     * Returns the vision-compensated version of the pose. Specifically, changes
     * the pose from being relative to this record's odometry pose to being
     * relative to this record's vision pose.
     *
     * @param pose The pose to compensate.
     * @return The compensated pose.
     */
    Pose3d Compensate(const Pose3d& pose) const {
      auto delta = pose - odometryPose;
      return visionPose + delta;
    }
  };

  static constexpr units::second_t kBufferDuration = 1.5_s;

  Odometry3d<WheelSpeeds, WheelPositions>& m_odometry;
  wpi::array<double, 4> m_q{wpi::empty_array};
  frc::Matrixd<6, 6> m_visionK = frc::Matrixd<6, 6>::Zero();

  // Maps timestamps to odometry-only pose estimates
  TimeInterpolatableBuffer<Pose3d> m_odometryPoseBuffer{kBufferDuration};
  // Maps timestamps to vision updates
  // Always contains one entry before the oldest entry in m_odometryPoseBuffer,
  // unless there have been no vision measurements after the last reset
  std::map<units::second_t, VisionUpdate> m_visionUpdates;

  Pose3d m_poseEstimate;
};

}  // namespace frc
