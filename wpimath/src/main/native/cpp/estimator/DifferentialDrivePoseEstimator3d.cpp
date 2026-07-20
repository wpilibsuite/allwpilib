// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/DifferentialDrivePoseEstimator3d.hpp"

#include "wpi/math/estimator/PoseEstimator3d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/array.hpp"

using namespace wpi::math;

DifferentialDrivePoseEstimator3d::DifferentialDrivePoseEstimator3d(
    DifferentialDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    wpi::units::meter_t leftDistance, wpi::units::meter_t rightDistance,
    const Pose3d& initialPose)
    : DifferentialDrivePoseEstimator3d{
          kinematics,          gyroAngle,   leftDistance,
          rightDistance,       initialPose, {0.02, 0.02, 0.02, 0.01},
          {0.1, 0.1, 0.1, 0.1}} {}

DifferentialDrivePoseEstimator3d::DifferentialDrivePoseEstimator3d(
    DifferentialDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    wpi::units::meter_t leftDistance, wpi::units::meter_t rightDistance,
    const Pose3d& initialPose, const wpi::util::array<double, 4>& stateStdDevs,
    const wpi::util::array<double, 4>& visionMeasurementStdDevs)
    : PoseEstimator3d(m_odometryImpl, stateStdDevs, visionMeasurementStdDevs),
      m_odometryImpl{gyroAngle, leftDistance, rightDistance, initialPose} {
  ResetPose(initialPose);
}
