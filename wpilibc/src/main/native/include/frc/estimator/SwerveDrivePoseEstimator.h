/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <iostream>
#include <limits>

#include <Eigen/Core>
#include <units/time.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/ExtendedKalmanFilter.h"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc2/Timer.h"

namespace frc {

template <int N>
using Vector = Eigen::Matrix<double, N, 1>;

/**
 * This class wraps an ExtendedKalmanFilter to fuse latency-compensated vision
 * measurements with swerve drive encoder velocity measurements. It will correct
 * for noisy measurements and encoder drift. It is intended to be an easy but
 * more accurate drop-in for SwerveDriveOdometry.
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
 * <strong> x = [[x, y, std::cos(theta), std::sin(theta)]]^T </strong> in the
 * field-coordinate system.
 *
 * <strong> u = [[vx, vy, omega]]^T </strong> in the field-coordinate system.
 *
 * <strong> y = [[x, y, std::cos(theta), std::sin(theta)]]^T </strong> in field
 * coords from vision, or <strong> y = [[cos(theta), std::sin(theta)]]^T
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
   *                                 wheel and gyro velocities less.
   * @param localMeasurementStdDevs  Standard deviations of the gyro
   *                                 measurement. Increase this number to
   *                                 trust gyro angle measurements less.
   * @param visionMeasurementStdDevs Standard deviations of the encoder
   *                                 measurements. Increase these numbers to
   *                                 trust vision less.
   * @param nominalDt                The time in seconds between each robot
   *                                 loop.
   */
  SwerveDrivePoseEstimator(const Rotation2d& gyroAngle,
                           const Pose2d& initialPose,
                           SwerveDriveKinematics<NumModules>& kinematics,
                           const Vector<3>& stateStdDevs,
                           const Vector<1>& localMeasurementStdDevs,
                           const Vector<3>& visionMeasurementStdDevs,
                           units::second_t nominalDt = 0.02_s)
      : m_observer(
            &SwerveDrivePoseEstimator::F,
            [](const Vector<4>& x, const Vector<3>& u) {
              return x.block<2, 1>(2, 0);
            },
            StdDevMatrixToArray<4>(frc::MakeMatrix<4, 1>(
                stateStdDevs(0), stateStdDevs(1), std::cos(stateStdDevs(2)),
                std::sin(stateStdDevs(2)))),
            StdDevMatrixToArray<2>(
                frc::MakeMatrix<2, 1>(std::cos(localMeasurementStdDevs(0)),
                                      std::sin(localMeasurementStdDevs(0)))),
            nominalDt),
        m_kinematics(kinematics),
        m_nominalDt(nominalDt) {
    // Construct R (covariances) matrix for vision measurements.
    Eigen::Matrix4d visionContR =
        frc::MakeCovMatrix<4>(StdDevMatrixToArray<4>(frc::MakeMatrix<4, 1>(
            visionMeasurementStdDevs(0), visionMeasurementStdDevs(1),
            std::cos(visionMeasurementStdDevs(2)),
            std::sin(visionMeasurementStdDevs(2)))));

    // Create and store discrete covariance matrix for vision measurements.
    m_visionDiscR = frc::DiscretizeR<4>(visionContR, m_nominalDt);

    // Create correction mechanism for vision measurements.
    m_visionCorrect = [&](const Vector<3>& u, const Vector<4>& y) {
      m_observer.Correct<4>(
          u, y, [](const Vector<4>& x, const Vector<3>& u) { return x; },
          m_visionDiscR);
    };

    // Set initial state.
    m_observer.SetXhat(PoseTo4dVector(initialPose));

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
    m_observer.SetXhat(PoseTo4dVector(pose));

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
                  Rotation2d(m_observer.Xhat(2), m_observer.Xhat(3)));
  }

  /**
   * Add a vision measurement to the Extended Kalman Filter. This will correct
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
    m_latencyCompensator.ApplyPastMeasurement<4>(
        &m_observer, m_nominalDt, PoseTo4dVector(visionRobotPose),
        m_visionCorrect, timestamp);
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder
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
    return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), gyroAngle,
                          moduleStates...);
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder
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

    auto localY =
        frc::MakeMatrix<2, 1>(std::cos(angle.Radians().template to<double>()),
                              std::sin(angle.Radians().template to<double>()));
    m_previousAngle = angle;

    m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);

    m_observer.Predict(u, dt);
    m_observer.Correct(u, localY);

    return GetEstimatedPosition();
  }

 private:
  ExtendedKalmanFilter<4, 3, 2> m_observer;
  SwerveDriveKinematics<NumModules>& m_kinematics;
  KalmanFilterLatencyCompensator<4, 3, 2, ExtendedKalmanFilter<4, 3, 2>>
      m_latencyCompensator;
  std::function<void(const Vector<3>& u, const Vector<4>& y)> m_visionCorrect;

  Eigen::Matrix4d m_visionDiscR;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  static Vector<4> F(const Vector<4>& x, const Vector<3>& u) {
    return frc::MakeMatrix<4, 1>(u(0), u(1), -x(3) * u(2), x(2) * u(2));
  }

  template <int Dim>
  static std::array<double, Dim> StdDevMatrixToArray(
      const Vector<Dim>& vector) {
    std::array<double, Dim> array;
    for (size_t i = 0; i < Dim; ++i) {
      array[i] = vector(i);
    }
    return array;
  }
};

}  // namespace frc
