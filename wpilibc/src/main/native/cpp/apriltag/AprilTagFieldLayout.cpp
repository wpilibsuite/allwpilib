// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagFieldLayout.h"

#include <algorithm>
#include <system_error>
#include "frc/geometry/Pose3d.h"

#include <units/angle.h>
#include <units/length.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

using namespace frc;

AprilTagFieldLayout::AprilTagFieldLayout(std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_istream input{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json;
  input >> json;

  m_apriltags = json.at("tags").get<std::vector<AprilTag>>();
  m_fieldWidth = units::meter_t{json.at("field").at("width").get<double>()};
  m_fieldLength = units::meter_t{json.at("field").at("height").get<double>()};
}

AprilTagFieldLayout::AprilTagFieldLayout(std::vector<AprilTag> apriltags,
                                         units::meter_t fieldLength,
                                         units::meter_t fieldWidth)
    : m_apriltags(std::move(apriltags)),
      m_fieldLength(std::move(fieldLength)),
      m_fieldWidth(std::move(fieldWidth)) {}

void AprilTagFieldLayout::SetAlliance(DriverStation::Alliance alliance) {
  m_mirror = alliance == DriverStation::Alliance::kRed;
}

std::optional<frc::Pose3d> AprilTagFieldLayout::GetTagPose(int ID) const {
  Pose3d returnPose;
  auto it = std::find_if(m_apriltags.begin(), m_apriltags.end(),
                         [=](const auto& tag) { return tag.ID == ID; });
  if (it != m_apriltags.end()) {
    returnPose = it->pose;
  } else {
    return std::optional<Pose3d>();
  }
  if (m_mirror) {
    returnPose = returnPose.RelativeTo(Pose3d{
        m_fieldLength, m_fieldWidth, 0_m, Rotation3d{0_deg, 0_deg, 180_deg}});
  }
  return std::make_optional(returnPose);
}

void AprilTagFieldLayout::Serialize(std::string_view path) {
  std::error_code error_code;

  wpi::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::json json = *this;
  output << json;
  output.flush();
}

bool AprilTagFieldLayout::operator==(const AprilTagFieldLayout& other) const {
  return m_apriltags == other.m_apriltags && m_mirror == other.m_mirror &&
         m_fieldLength == other.m_fieldLength &&
         m_fieldWidth == other.m_fieldWidth;
}

bool AprilTagFieldLayout::operator!=(const AprilTagFieldLayout& other) const {
  return !operator==(other);
}

void frc::to_json(wpi::json& json, const AprilTagFieldLayout& layout) {
  json = wpi::json{{"field",
                    {{"length", layout.m_fieldLength.value()},
                     {"width", layout.m_fieldWidth.value()}}},
                   {"tags", layout.m_apriltags}};
}

void frc::from_json(const wpi::json& json, AprilTagFieldLayout& layout) {
  layout.m_apriltags = json.at("tags").get<std::vector<AprilTag>>();
  layout.m_fieldLength =
      units::meter_t{json.at("field").at("length").get<double>()};
  layout.m_fieldWidth =
      units::meter_t{json.at("field").at("width").get<double>()};
}
