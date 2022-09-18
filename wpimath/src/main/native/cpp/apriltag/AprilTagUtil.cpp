// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagUtil.h"

#include <system_error>
#include <vector>

#include <fmt/format.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

using namespace frc;

bool AprilTagUtil::AprilTag::operator==(const AprilTag &other) const {
    return id == other.id && pose == other.pose;
}

bool AprilTagUtil::AprilTag::operator!=(const AprilTag &other) const {
    return !operator==(other);
}

void AprilTagUtil::ToJson(const std::vector<AprilTag>& apriltagFieldLayout,
                                      std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json = apriltagFieldLayout;
  output << json;
  output.flush();
}

std::vector<AprilTagUtil::AprilTag> AprilTagUtil::FromJson(std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_istream input{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json;
  input >> json;

  return json.get<std::vector<AprilTagUtil::AprilTag>>();
}

std::string AprilTagUtil::SerializeAprilTagLayout(const std::vector<AprilTagUtil::AprilTag>& apriltagLayout) {
  wpi::json json = apriltagLayout;
  return json.dump();
}

std::vector<AprilTagUtil::AprilTag> AprilTagUtil::DeserializeAprilTagLayout(std::string_view jsonStr) {
  wpi::json json = wpi::json::parse(jsonStr);
  return json.get<std::vector<AprilTagUtil::AprilTag>>();
}

void frc::to_json(wpi::json& json, const AprilTagUtil::AprilTag& apriltag) {
    json = wpi::json{
        {"id", apriltag.id},
        {"pose", apriltag.pose}
    };
}

void frc::from_json(const wpi::json& json, AprilTagUtil::AprilTag& apriltag) {
    apriltag.id = json.at("id").get<int>();
    apriltag.pose = json.at("pose").get<Pose3d>();
}