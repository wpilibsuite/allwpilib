// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include <wpi/array.h>
#include <wpi/timestamp.h>

#include "Eigen/Core"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
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
 * slower than the default of 0.02s, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 *
 * Our state-space system is:
 *
 * <strong> x = [[x, y, theta]]^T </strong> in the
 * field-coordinate system.
 *
 * <strong> u = [[vx, vy, omega]]^T </strong> in the field-coordinate system.
 *
 * <strong> y = [[x, y, std::theta]]^T </strong> in field
 * coords from vision, or <strong> y = [[theta]]^T
 * </strong> from the gyro.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator {
 public:
  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param kinematics               A correctly-configured kinematics object
   *                                 for your drivetrain.
   * @param stateStdDevs             Standard deviations of model states.
   *                                 Increase these numbers to trust your
   *                                 model's state estimates less. This matrix
   *                                 is in the form [x, y, theta]^T, with units
   *                                 in meters and radians.
   * @param localMeasurementStdDevs  Standard deviations of the encoder and gyro
   *                                 measurements. Increase these numbers to
   *                                 trust sensor readings from encoders
   *                                 and gyros less. This matrix is in the form
   *                                 [theta], with units in radians.
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]^T, with units in meters and
   *                                 radians.
   * @param nominalDt                The time in seconds between each robot
   *                                 loop.
   */
  SwerveDrivePoseEstimator(
      const Rotation2d& gyroAngle, const Pose2d& initialPose,
      SwerveDriveKinematics<NumModules>& kinematics,
      const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 1>& localMeasurementStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs,
      units::second_t nominalDt = 0.02_s)
      : m_observer([](const Eigen::Matrix<double, 3, 1>& x,
                      const Eigen::Matrix<double, 3, 1>& u) { return u; },
                   [](const Eigen::Matrix<double, 3, 1>& x,
                      const Eigen::Matrix<double, 3, 1>& u) {
                     return x.block<1, 1>(2, 0);
                   },
                   stateStdDevs, localMeasurementStdDevs,
                   frc::AngleMean<3, 3>(2), frc::AngleMean<1, 3>(0),
                   frc::AngleResidual<3>(2), frc::AngleResidual<1>(0),
                   frc::AngleAdd<3>(2), nominalDt),
        m_kinematics(kinematics),
        m_nominalDt(nominalDt) {
    SetVisionMeasurementStdDevs(visionMeasurementStdDevs);

    // Create correction mechanism for vision measurements.
    m_visionCorrect = [&](const Eigen::Matrix<double, 3, 1>& u,
                          const Eigen::Matrix<double, 3, 1>& y) {
      m_observer.Correct<3>(
          u, y,
          [](const Eigen::Matrix<double, 3, 1>& x,
             const Eigen::Matrix<double, 3, 1>& u) { return x; },
          m_visionDiscR, frc::AngleMean<3, 3>(2), frc::AngleResidual<3>(2),
          frc::AngleResidual<3>(2), frc::AngleAdd<3>(2));
    };

    // Set initial state.
    m_observer.SetXhat(PoseTo3dVector(initialPose));

    // Calculate offsets.
    m_gyroOffset = initialPose.Rotation() - gyroAngle;
    m_previousAngle = initialPose.Rotation();
  }

  /**
   * Resets the robot's position on the field.
   *
   * You NEED to reset your encoders (to zero) when calling this method.
   *
   * The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param pose      The position on the field that your robot is at.
   * @param gyroAngle The angle reported by the gyroscope.
   */
  void ResetPosition(const Pose2d& pose, const Rotation2d& gyroAngle) {
    // Set observer state.
    m_observer.SetXhat(PoseTo3dVector(pose));

    // Calculate offsets.
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
    return Pose2d(m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                  Rotation2d(units::radian_t{m_observer.Xhat(2)}));
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
   *                                 [x, y, theta]^T, with units in meters and
   *                                 radians.
   */
  void SetVisionMeasurementStdDevs(
      const wpi::array<double, 3>& visionMeasurementStdDevs) {
    // Create R (covariances) for vision measurements.
    Eigen::Matrix<double, 3, 3> visionContR =
        frc::MakeCovMatrix(visionMeasurementStdDevs);
    m_visionDiscR = frc::DiscretizeR<3>(visionContR, m_nominalDt);
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct
   * the odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
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
    m_latencyCompensator.ApplyPastMeasurement<3>(
        &m_observer, m_nominalDt, PoseTo3dVector(visionRobotPose),
        m_visionCorrect, timestamp);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param gyroAngle    The current gyro angle.
   * @param moduleStates The current velocities and rotations of the swerve
   *                     modules.
   * @return The estimated pose of the robot in meters.
   */
  template <typename... ModuleState>
  Pose2d Update(const Rotation2d& gyroAngle, ModuleState&&... moduleStates) {
    return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                          moduleStates...);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param currentTime  Time at which this method was called, in seconds.
   * @param gyroAngle    The current gyroscope angle.
   * @param moduleStates The current velocities and rotations of the swerve
   *                     modules.
   * @return The estimated pose of the robot in meters.
   */
  template <typename... ModuleState>
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        ModuleState&&... moduleStates) {
    auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
    m_prevTime = currentTime;

    auto angle = gyroAngle + m_gyroOffset;
    auto omega = (angle - m_previousAngle).Radians() / dt;

    auto chassisSpeeds = m_kinematics.ToChassisSpeeds(moduleStates...);
    auto fieldRelativeSpeeds =
        Translation2d(chassisSpeeds.vx * 1_s, chassisSpeeds.vy * 1_s)
            .RotateBy(angle);

    auto u =
        frc::MakeMatrix<3, 1>(fieldRelativeSpeeds.X().template to<double>(),
                              fieldRelativeSpeeds.Y().template to<double>(),
                              omega.template to<double>());

    auto localY = frc::MakeMatrix<1, 1>(angle.Radians().template to<double>());
    m_previousAngle = angle;

    m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);

    m_observer.Predict(u, dt);
    m_observer.Correct(u, localY);

    return GetEstimatedPosition();
  }

 private:
  UnscentedKalmanFilter<3, 3, 1> m_observer;
  SwerveDriveKinematics<NumModules>& m_kinematics;
  KalmanFilterLatencyCompensator<3, 3, 1, UnscentedKalmanFilter<3, 3, 1>>
      m_latencyCompensator;
  std::function<void(const Eigen::Matrix<double, 3, 1>& u,
                     const Eigen::Matrix<double, 3, 1>& y)>
      m_visionCorrect;

  Eigen::Matrix3d m_visionDiscR;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  template <int Dim>
  static wpi::array<double, Dim> StdDevMatrixToArray(
      const Eigen::Matrix<double, Dim, 1>& vector) {
    wpi::array<double, Dim> array;
    for (size_t i = 0; i < Dim; ++i) {
      array[i] = vector(i);
    }
    return array;
  }
};

}  // namespace frc
