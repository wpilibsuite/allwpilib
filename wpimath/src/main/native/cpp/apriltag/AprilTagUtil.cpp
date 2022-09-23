// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagUtil.h"

#include <system_error>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "frc/geometry/Pose3d.h"

using namespace frc;

std::string AprilTagUtil::SerializeAprilTagLayout(
    const std::vector<std::pair<int, Pose3d>>& apriltagLayout) {
  wpi::json json = apriltagLayout;
  return json.at("tags").dump();
}

std::vector<std::pair<int, Pose3d>> AprilTagUtil::DeserializeAprilTagLayout(
    std::string_view jsonStr) {
  wpi::json json = wpi::json::parse(jsonStr);
  return json.at("tags").get<std::vector<std::pair<int, Pose3d>>>();
}
