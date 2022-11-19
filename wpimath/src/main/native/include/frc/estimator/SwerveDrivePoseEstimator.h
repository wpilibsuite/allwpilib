// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>
#include <wpi/timestamp.h>

#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"
#include "frc/estimator/KalmanFilter.h"
#include "frc/system/LinearSystem.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry.h"
#include "units/time.h"
#include "fmt/format.h"

namespace frc {
/**
 * This class wraps Swerve Drive Odometry to fuse latency-compensated
 * vision measurements with swerve drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be a
 * drop-in for SwerveDriveOdometry.
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 *
 * The state-space system used internally has the following states (x) and outputs (y):
 *
 * <strong> x = [x, y, theta]ᵀ </strong> in the field coordinate
 * system containing x position, y position, and heading.
 *
 * <strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y
 * position, and heading.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator {
 public:
  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param modulePositions          The current distance and rotation
   *                                 measurements of the swerve modules.
   * @param initialPose              The starting pose estimate.
   * @param kinematics               A correctly-configured kinematics object
   *                                 for your drivetrain.
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
   * @param nominalDt                The time in seconds between each robot
   *                                 loop.
   */
  SwerveDrivePoseEstimator(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose, 
      SwerveDriveKinematics<NumModules>& kinematics,
      const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs,
      units::second_t nominalDt = 20_ms)
      : m_odometry{kinematics, gyroAngle, modulePositions, initialPose},
        m_nominalDt(nominalDt),
        m_previousGyroAngle(gyroAngle),
        m_stateStdDevs(stateStdDevs) {

    for (size_t i = 0; i < NumModules; i++) {
      m_prevModulePositions[i] = {modulePositions[i].distance, modulePositions[i].angle};
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

    m_previousGyroAngle = gyroAngle;
    for (size_t i = 0; i < NumModules; i++) {
      m_prevModulePositions[i].distance = modulePositions[i].distance;
    }
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Extended
   * Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const {
    return m_odometry.GetPose();
  }

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
      const wpi::array<double, 3>& visionMeasurementStdDevs) {
    auto system = LinearSystem<3, 3, 3>{
      frc::Matrixd<3, 3>::Identity(),
      frc::Matrixd<3, 3>::Zero(),
      frc::Matrixd<3, 3>::Identity(),
      frc::Matrixd<3, 3>::Zero()
    };
    auto visionObserver = KalmanFilter<3, 3, 3>{
       system,
       m_stateStdDevs,
       visionMeasurementStdDevs,
       m_nominalDt
    };
    // Create R (covariances) for vision measurements.
    m_visionK = visionObserver.K();
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct
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
                            units::second_t timestamp) {
    // Step 1: Get the estimated pose from when the vision measurement was made.
    auto sample = m_poseBuffer.Sample(timestamp);

    if (!sample.has_value()) {
      return;
    }

    // Step 2: Measure the twist between the odometry pose and the vision pose
    auto twist = sample.value().Log(visionRobotPose);

    // Step 3: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    auto k_times_twist = m_visionK * frc::Vectord<3>{twist.dx.value(), twist.dy.value(), twist.dtheta.value()};

    // Step 4: Convert back to Twist2d
    auto scaled_twist = Twist2d{units::meter_t{k_times_twist(0)}, units::meter_t{k_times_twist(1)}, units::radian_t{k_times_twist(2)}};

    // Step 5: Apply scaled twist to the latest pose
    auto est_pose = GetEstimatedPosition().Exp(scaled_twist);

    // Step 6: Apply new pose to odometry
    m_odometry.ResetPosition(m_previousGyroAngle, m_prevModulePositions, est_pose);
  }

  /**
   * Adds a vision measurement to the Unscented Kalman Filter. This will correct
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
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param gyroAngle       The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *                        the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d Update(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                          modulePositions);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param currentTime     Time at which this method was called, in seconds.
   * @param gyroAngle       The current gyro angle.
   * @param modulePositions The current distance travelled and rotations of
   *                        the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d UpdateWithTime(
      units::second_t currentTime, const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());
    m_odometry.Update(gyroAngle, modulePositions);

    m_previousGyroAngle = gyroAngle;
    for (size_t i = 0; i < NumModules; i++) {
      m_prevModulePositions[i].distance = modulePositions[i].distance;
    }

    return GetEstimatedPosition();
  }

 private:
  SwerveDriveOdometry<NumModules> m_odometry;
  TimeInterpolatableBuffer<Pose2d> m_poseBuffer{1.5_s};

  Eigen::Matrix3d m_visionK;

  units::second_t m_nominalDt;

  Rotation2d m_previousGyroAngle;
  wpi::array<SwerveModulePosition, NumModules> m_prevModulePositions{wpi::empty_array};
  const wpi::array<double, 3>& m_stateStdDevs;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDrivePoseEstimator<4>;

}  // namespace frc
