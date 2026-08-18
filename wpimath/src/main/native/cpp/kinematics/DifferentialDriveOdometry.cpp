// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry.hpp"

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/kinematics/Odometry.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/length.hpp"

using namespace wpi::math;

DifferentialDriveOdometry::DifferentialDriveOdometry(
    const Rotation2d& gyroAngle, wpi::units::meter_t leftDistance,
    wpi::units::meter_t rightDistance, const Pose2d& initialPose)
    : Odometry(DifferentialDriveKinematics{1_m}, gyroAngle,
               {leftDistance, rightDistance}, initialPose) {
  wpi::math::MathSharedStore::ReportUsage("DifferentialDriveOdometry", "");
}
