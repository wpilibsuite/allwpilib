// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry3d.hpp"

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi::math;

DifferentialDriveOdometry3d::DifferentialDriveOdometry3d(
    const Rotation3d& gyroAngle, wpi::units::meters<> leftDistance,
    wpi::units::meters<> rightDistance, const Pose3d& initialPose)
    : Odometry3d(DifferentialDriveKinematics{1_m}, gyroAngle,
                 {leftDistance, rightDistance}, initialPose) {
  wpi::util::ReportUsage("DifferentialDriveOdometry3d", "");
}
