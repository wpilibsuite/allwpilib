// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AprilTagFieldLayout.h"

#include <algorithm>
#include <string>

#include <units/angle.h>
#include <units/length.h>
#include <wpi/json.h>

#include "frc/DriverStation.h"
#include "frc/apriltag/AprilTagUtil.h"
#include "frc/geometry/Pose3d.h"

using namespace frc;

AprilTagFieldLayout::AprilTagFieldLayout(
    const std::vector<AprilTagUtil::AprilTag>& apriltags)
    : m_apriltags(std::move(apriltags)) {}

frc::Pose3d AprilTagFieldLayout::GetTagPose(int id) const {
  Pose3d returnPose;
  auto it = std::find_if(m_apriltags.begin(), m_apriltags.end(),
                         [=](const auto& tag) { return tag.id == id; });
  if (it != m_apriltags.end()) {
    returnPose = it->pose;
  }
  if (m_mirror) {
    returnPose = returnPose.RelativeTo(
        Pose3d{54_ft, 27_ft, 0_ft, Rotation3d{0_deg, 0_deg, 180_deg}});
  }
  return returnPose;
}

void AprilTagFieldLayout::SetAlliance(DriverStation::Alliance alliance) {
  m_mirror = alliance == DriverStation::Alliance::kRed;
}
