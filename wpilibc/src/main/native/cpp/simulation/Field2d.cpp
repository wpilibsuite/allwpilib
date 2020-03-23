/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/Field2d.h"

using namespace frc;

Field2d::Field2d() : m_device{"Field2D"} {
  if (m_device) {
    m_x = m_device.CreateDouble("x", false, 0.0);
    m_y = m_device.CreateDouble("y", false, 0.0);
    m_rot = m_device.CreateDouble("rot", false, 0.0);
  }
}

void Field2d::SetRobotPose(const Pose2d& pose) {
  if (m_device) {
    auto& translation = pose.Translation();
    m_x.Set(translation.X().to<double>());
    m_y.Set(translation.Y().to<double>());
    m_rot.Set(pose.Rotation().Degrees().to<double>());
  } else {
    m_pose = pose;
  }
}

void Field2d::SetRobotPose(units::meter_t x, units::meter_t y,
                           Rotation2d rotation) {
  if (m_device) {
    m_x.Set(x.to<double>());
    m_y.Set(y.to<double>());
    m_rot.Set(rotation.Degrees().to<double>());
  } else {
    m_pose = Pose2d{x, y, rotation};
  }
}

Pose2d Field2d::GetRobotPose() {
  if (m_device) {
    return Pose2d{units::meter_t{m_x.Get()}, units::meter_t{m_y.Get()},
                  Rotation2d{units::degree_t{m_rot.Get()}}};
  } else {
    return m_pose;
  }
}
