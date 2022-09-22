// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/DriverStation.h"
#include "frc/apriltag/AprilTagUtil.h"
#include "frc/geometry/Pose3d.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
  AprilTagFieldLayout() = default;
  
  explicit AprilTagFieldLayout(const std::string_view path);

  explicit AprilTagFieldLayout(
      const std::vector<AprilTagUtil::AprilTag>& apriltags);

  Pose3d GetTagPose(int id) const;

  void SetAlliance(DriverStation::Alliance alliance);
  
  void ToJson(const std::vector<AprilTagUtil::AprilTag>& apriltagLayout,
                          std::string_view path);

 private:
  std::vector<AprilTagUtil::AprilTag> m_apriltags;
  bool m_mirror;
};
}  // namespace frc
