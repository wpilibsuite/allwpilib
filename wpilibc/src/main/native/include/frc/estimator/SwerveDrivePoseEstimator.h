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
#include <units/units.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/KalmanFilter.h"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc2/Timer.h"

namespace frc {

template <int N>
using Vector = Eigen::Matrix<double, N, 1>;

template <unsigned int NumModules>
class SwerveDrivePoseEstimator {
 public:
  SwerveDrivePoseEstimator(const Rotation2d& gyroAngle,
                           const Pose2d& initialPose,
                           const SwerveDriveKinematics<NumModules>& kinematics,
                           const Vector<3>& stateStdDevs,
                           const Vector<3>& measurementStdDevs,
                           units::second_t nominalDt = 0.02_s)
      : m_observer(GetObserverSystem(), StdDevMatrixToArray(stateStdDevs),
                   StdDevMatrixToArray(measurementStdDevs), nominalDt),
        m_kinematics(kinematics),
        m_nominalDt(nominalDt) {
    m_gyroOffset = initialPose.Rotation() - gyroAngle;
    m_previousAngle = initialPose.Rotation();
    m_observer.SetXhat(PoseToVector(initialPose));
  }

  void ResetPosition(const Pose2d& pose, const Rotation2d& gyroAngle) {
    m_previousAngle = pose.Rotation();
    m_gyroOffset = GetEstimatedPosition().Rotation() - gyroAngle;
  }

  Pose2d GetEstimatedPosition() const {
    return Pose2d(units::meter_t(m_observer.Xhat(0)),
                  units::meter_t(m_observer.Xhat(1)),
                  Rotation2d(units::radian_t(m_observer.Xhat(2))));
  }

  void AddVisionMeasurement(const Pose2d& visionRobotPose,
                            units::second_t timestamp) {
    m_latencyCompensator.ApplyPastMeasurement(
        m_observer, m_nominalDt, PoseToVector(visionRobotPose), timestamp);
  }

  template <typename... ModuleStates>
  Pose2d Update(const Rotation2d& gyroAngle, ModuleStates&&... moduleStates) {
    return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), gyroAngle,
                          moduleStates...);
  }

  template <typename... ModuleStates>
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        ModuleStates&&... moduleStates) {
    auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
    m_prevTime = currentTime;

    auto angle = gyroAngle + m_gyroOffset;
    auto omega = (angle - m_previousAngle).Radians() / dt;

    auto chassisSpeeds = m_kinematics.ToChassisSpeeds(moduleStates...);
    Translation2d fieldRelativeVelocities =
        Translation2d(chassisSpeeds.vx * 1_s, chassisSpeeds.vy * 1_s)
            .RotateBy(angle);

    auto u = frc::MakeMatrix<3, 1>(fieldRelativeVelocities.X().to<double>(),
                                   fieldRelativeVelocities.Y().to<double>(),
                                   omega.to<double>());
    m_previousAngle = angle;

    m_latencyCompensator.AddObserverState(m_observer, u, currentTime);
    m_observer.Predict(u, dt);

    return GetEstimatedPosition();
  }

 private:
  KalmanFilter<3, 3, 3> m_observer;
  KalmanFilterLatencyCompensator<3, 3, 3, KalmanFilter<3, 3, 3>>
      m_latencyCompensator;
  SwerveDriveKinematics<NumModules> m_kinematics;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  static LinearSystem<3, 3, 3>& GetObserverSystem() {
    static LinearSystem<3, 3, 3> observerSystem{
        Eigen::MatrixXd::Zero(3, 3),
        Eigen::MatrixXd::Identity(3, 3),
        Eigen::MatrixXd::Identity(3, 3),
        Eigen::MatrixXd::Zero(3, 3),
        frc::MakeMatrix<3, 1>(-std::numeric_limits<double>::max(),
                              -std::numeric_limits<double>::max(),
                              -std::numeric_limits<double>::max()),
        frc::MakeMatrix<3, 1>(std::numeric_limits<double>::max(),
                              std::numeric_limits<double>::max(),
                              std::numeric_limits<double>::max())};
    return observerSystem;
  }

  static std::array<double, 3> StdDevMatrixToArray(const Vector<3>& stdDevs) {
    return std::array<double, 3>{stdDevs(0, 0), stdDevs(1, 0), stdDevs(2, 0)};
  }
};

}  // namespace frc
