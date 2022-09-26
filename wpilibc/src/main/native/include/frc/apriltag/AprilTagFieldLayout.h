// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/DriverStation.h"
#include "frc/apriltag/AprilTag.h"
#include "frc/geometry/Pose3d.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
  AprilTagFieldLayout() = default;

  explicit AprilTagFieldLayout(std::string_view path);

  explicit AprilTagFieldLayout(const std::vector<AprilTag>& apriltags);

  Pose3d GetTagPose(int id) const;

  void SetAlliance(DriverStation::Alliance alliance);

  void Serialize(std::string_view path);

  bool operator==(const AprilTagFieldLayout& other) const;

  bool operator!=(const AprilTagFieldLayout& other) const;

 private:
  std::vector<AprilTag> m_apriltags;
  bool m_mirror = false;

  friend WPILIB_DLLEXPORT void to_json(wpi::json& json, const AprilTagFieldLayout& layout);

  friend WPILIB_DLLEXPORT void from_json(const wpi::json& json, AprilTagFieldLayout& layout);
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTagFieldLayout& layout);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTagFieldLayout& layout);

}  // namespace frc
