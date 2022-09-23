// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagFieldLayout.h"

#include <algorithm>
#include <string_view>
#include <system_error>
#include <vector>

#include <units/angle.h>
#include <units/length.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/geometry/Pose3d.h"

using namespace frc;

AprilTagFieldLayout::AprilTagFieldLayout(const std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_istream input{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json;
  input >> json;

  m_apriltags = json.get<std::vector<AprilTag>>();
}

AprilTagFieldLayout::AprilTagFieldLayout(const std::vector<AprilTag>& apriltags)
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

void ToJson(const AprilTagFieldLayout& apriltagLayout, std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json = apriltagLayout;
  output << json;
  output.flush();
}

void frc::to_json(wpi::json& json, const AprilTagFieldLayout& layout) {
  json = wpi::json{{"tags", layout.m_apriltags}};
}

void frc::from_json(const wpi::json& json, AprilTagFieldLayout& layout) {
  layout.m_apriltags = json.at("tags").get<std::vector<AprilTag>>();
}
