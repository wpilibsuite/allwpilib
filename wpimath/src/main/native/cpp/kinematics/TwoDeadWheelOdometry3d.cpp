// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/TwoDeadWheelOdometry3d.hpp"

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi::math;

TwoDeadWheelOdometry3d::TwoDeadWheelOdometry3d(wpi::units::meter_t xWheelYPos,
                                               wpi::units::meter_t yWheelXPos,
                                               wpi::units::meter_t xWheelPos,
                                               wpi::units::meter_t yWheelPos,
                                               const Rotation3d& gyroAngle,
                                               const Pose3d& initialPose)
    : m_xWheelYPos(xWheelYPos),
      m_yWheelXPos(yWheelXPos),
      m_pose(initialPose),
      m_previousXWheelPos(xWheelPos),
      m_previousYWheelPos(yWheelPos),
      m_previousGyroAngle(gyroAngle) {
  wpi::util::ReportUsage("TwoDeadWheelOdometry3d", "");
}
