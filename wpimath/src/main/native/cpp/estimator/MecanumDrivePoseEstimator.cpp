// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/MecanumDrivePoseEstimator.hpp"

#include "wpi/math/estimator/PoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"
#include "wpi/util/array.hpp"

using namespace wpi::math;

wpi::math::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose)
    : MecanumDrivePoseEstimator{kinematics,      gyroAngle,
                                wheelPositions,  initialPose,
                                {0.1, 0.1, 0.1}, {0.45, 0.45, 0.45}} {}

wpi::math::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose,
    const wpi::util::array<double, 3>& stateStdDevs,
    const wpi::util::array<double, 3>& visionMeasurementStdDevs)
    : PoseEstimator(kinematics, m_odometryImpl, stateStdDevs,
                    visionMeasurementStdDevs),
      m_odometryImpl(kinematics, gyroAngle, wheelPositions, initialPose) {
  ResetPose(initialPose);
}
