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
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/interpolation/TimeInterpolatableBuffer.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps an Unscented Kalman Filter to fuse latency-compensated
 * vision measurements with swerve drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an
 * easy but more accurate drop-in for SwerveDriveOdometry.
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 *
 * The state-space system used internally has the following states (x), inputs
 * (u), and outputs (y):
 *
 * <strong> x = [x, y, theta, s_0, ..., s_n]ᵀ </strong> in the field coordinate
 * system containing x position, y position, and heading, followed by the
 * distance travelled by each wheel.
 *
 * <strong> u = [v_x, v_y, omega, v_0, ... v_n]ᵀ </strong> containing x
 * velocity, y velocity, and angular velocity in the field coordinate system,
 * followed by the velocity measured at each wheel.
 *
 * <strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y
 * position, and heading; or <strong> y = [theta, s_0, ..., s_n]ᵀ </strong>
 * containing gyro heading, followed by the distance travelled by each wheel.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator {
 public:
  static constexpr size_t States = 3 + NumModules;
  static constexpr size_t Inputs = 3 + NumModules;
  static constexpr size_t Outputs = 1 + NumModules;

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
   *                                 is in the form [x, y, theta, s_0, ...
   * s_n]ᵀ, with units in meters and radians, then meters.
   * @param localMeasurementStdDevs  Standard deviation of the gyro measurement.
   *                                 Increase this number to trust sensor
   *                                 readings from the gyro less. This matrix is
   *                                 in the form [theta, s_0, ... s_n], with
   * units in radians followed by meters.
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
      const Pose2d& initialPose, SwerveDriveKinematics<NumModules>& kinematics,
      const wpi::array<double, States>& stateStdDevs,
      const wpi::array<double, Outputs>& localMeasurementStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs,
      units::second_t nominalDt = 20_ms)
      : m_observer([](const Vectord<States>& x,
                      const Vectord<Inputs>& u) { return u; },
                   [](const Vectord<States>& x, const Vectord<Inputs>& u) {
                     return x.template block<States - 2, 1>(2, 0);
                   },
                   stateStdDevs, localMeasurementStdDevs,
                   frc::AngleMean<States, States>(2),
                   frc::AngleMean<Outputs, States>(0),
                   frc::AngleResidual<States>(2),
                   frc::AngleResidual<Outputs>(0), frc::AngleAdd<States>(2),
                   nominalDt),
        m_kinematics(kinematics),
        m_nominalDt(nominalDt) {
    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);

    // Create correction mechanism for vision measurements.
    m_visionCorrect = [&](const Vectord<Inputs>& u, const Vectord<3>& y) {
      m_observer.template Correct<3>(
          u, y,
          [](const Vectord<States>& x, const Vectord<Inputs>& u) {
            return x.template block<3, 1>(0, 0);
          },
          m_visionContR, frc::AngleMean<3, States>(2), frc::AngleResidual<3>(2),
          frc::AngleResidual<States>(2), frc::AngleAdd<States>(2));
    };

    // Set initial state.
    Vectord<States> xhat;
    auto poseVec = PoseTo3dVector(initialPose);
    xhat(0) = poseVec(0);
    xhat(1) = poseVec(1);
    xhat(2) = poseVec(2);
    for (size_t i = 0; i < NumModules; i++) {
      xhat(3 + i) = modulePositions[i].distance.value();
    }
    m_observer.SetXhat(xhat);

    // Calculate offsets.
    m_gyroOffset = initialPose.Rotation() - gyroAngle;
    m_previousAngle = initialPose.Rotation();
  }

  /**
   * Resets the robot's position on the field.
   *
   * IF leftDistance and rightDistance are unspecified,
   * You NEED to reset your encoders (to zero).
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
    m_observer.Reset();
    m_poseBuffer.Clear();

    Vectord<States> xhat;
    auto poseVec = PoseTo3dVector(pose);
    xhat(0) = poseVec(0);
    xhat(1) = poseVec(1);
    xhat(2) = poseVec(2);
    for (size_t i = 0; i < NumModules; i++) {
      xhat(3 + i) = modulePositions[i].distance.value();
    }
    m_observer.SetXhat(xhat);

    m_prevTime = -1_s;

    m_gyroOffset = pose.Rotation() - gyroAngle;
    m_previousAngle = pose.Rotation();
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Extended
   * Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const {
    return Pose2d{m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                  Rotation2d{units::radian_t{m_observer.Xhat(2)}}};
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
    // Create R (covariances) for vision measurements.
    m_visionContR = frc::MakeCovMatrix(visionMeasurementStdDevs);
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
    if (auto sample = m_poseBuffer.Sample(timestamp)) {
      m_visionCorrect(Vectord<States>::Zero(),
                      PoseTo3dVector(GetEstimatedPosition().TransformBy(
                          visionRobotPose - sample.value())));
    }
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
   * @param moduleStates    The current velocities and rotations of the swerve
   *                        modules.
   * @param modulePositions The current distance and rotation measurements of
   *                        the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d Update(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModuleState, NumModules>& moduleStates,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                          moduleStates, modulePositions);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param currentTime     Time at which this method was called, in seconds.
   * @param gyroAngle       The current gyro angle.
   * @param moduleStates    The current velocities and rotations of the swerve
   *                        modules.
   * @param modulePositions The current distance travelled and rotations of
   *                        the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d UpdateWithTime(
      units::second_t currentTime, const Rotation2d& gyroAngle,
      const wpi::array<SwerveModuleState, NumModules>& moduleStates,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
    m_prevTime = currentTime;

    auto angle = gyroAngle + m_gyroOffset;
    auto omega = (angle - m_previousAngle).Radians() / dt;

    auto chassisSpeeds = m_kinematics.ToChassisSpeeds(moduleStates);
    auto fieldRelativeSpeeds =
        Translation2d{chassisSpeeds.vx * 1_s, chassisSpeeds.vy * 1_s}.RotateBy(
            angle);

    Vectord<Inputs> u;
    u(0) = fieldRelativeSpeeds.X().value();
    u(1) = fieldRelativeSpeeds.Y().value();
    u(2) = omega.value();
    for (size_t i = 0; i < NumModules; i++) {
      u(3 + i) = moduleStates[i].speed.value();
    }

    Vectord<Outputs> localY;
    localY(0) = angle.Radians().value();
    for (size_t i = 0; i < NumModules; i++) {
      localY(1 + i) = modulePositions[i].distance.value();
    }

    m_previousAngle = angle;

    m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());

    m_observer.Predict(u, dt);
    m_observer.Correct(u, localY);

    return GetEstimatedPosition();
  }

 private:
  UnscentedKalmanFilter<States, Inputs, Outputs> m_observer;
  SwerveDriveKinematics<NumModules>& m_kinematics;
  TimeInterpolatableBuffer<Pose2d> m_poseBuffer{1.5_s};
  std::function<void(const Vectord<Inputs>& u, const Vectord<3>& y)>
      m_visionCorrect;

  Eigen::Matrix3d m_visionContR;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDrivePoseEstimator<4>;

}  // namespace frc
