// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagUtil.h"

#include <system_error>
#include "frc/apriltag/AprilTagFieldLayout.h"

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

using namespace frc;

void AprilTagUtil::ToJson(const AprilTagFieldLayout& apriltagFieldLayout,
                                      std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json = apriltagFieldLayout.GetTags();
  output << json;
  output.flush();
}

AprilTagFieldLayout AprilTagUtil::FromJson(std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_istream input{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json;
  input >> json;

  return AprilTagFieldLayout{json.get<std::vector<AprilTagFieldLayout::AprilTag>>()};
}

std::string AprilTagUtil::SerializeAprilTagLayout(const AprilTagFieldLayout& apriltagFieldLayout) {
  wpi::json json = apriltagFieldLayout.GetTags();
  return json.dump();
}

AprilTagFieldLayout AprilTagUtil::DeserializeAprilTagLayout(std::string_view jsonStr) {
  wpi::json json = wpi::json::parse(jsonStr);
  return AprilTagFieldLayout{json.get<std::vector<AprilTagFieldLayout::AprilTag>>()};
}
