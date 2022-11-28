// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagFieldLayout.h"

#include <system_error>

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

  for (const auto& tag : json.at("tags").get<std::vector<AprilTag>>()) {
    m_apriltags[tag.ID] = tag;
  }
  m_fieldWidth = units::meter_t{json.at("field").at("width").get<double>()};
  m_fieldLength = units::meter_t{json.at("field").at("height").get<double>()};
}

AprilTagFieldLayout::AprilTagFieldLayout(std::vector<AprilTag> apriltags,
                                         units::meter_t fieldLength,
                                         units::meter_t fieldWidth)
    : m_fieldLength(std::move(fieldLength)),
      m_fieldWidth(std::move(fieldWidth)) {
  for (const auto& tag : apriltags) {
    m_apriltags[tag.ID] = tag;
  }
}

void AprilTagFieldLayout::SetOrigin(OriginPosition origin) {
  switch (origin) {
    case OriginPosition::kBlueAllianceWallRightSide:
      SetOrigin(Pose3d{});
      break;
    case OriginPosition::kRedAllianceWallRightSide:
      SetOrigin(Pose3d{Translation3d{m_fieldLength, m_fieldWidth, 0_m},
                       Rotation3d{0_deg, 0_deg, 180_deg}});
      break;
    default:
      throw std::invalid_argument("Invalid origin");
  }
}

void AprilTagFieldLayout::SetOrigin(const Pose3d& origin) {
  m_origin = origin;
}

std::optional<frc::Pose3d> AprilTagFieldLayout::GetTagPose(int ID) const {
  const auto& it = m_apriltags.find(ID);
  if (it == m_apriltags.end()) {
    return std::nullopt;
  }
  return it->second.pose.RelativeTo(m_origin);
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

void frc::to_json(wpi::json& json, const AprilTagFieldLayout& layout) {
  std::vector<AprilTag> tagVector;
  tagVector.reserve(layout.m_apriltags.size());
  for (const auto& pair : layout.m_apriltags) {
    tagVector.push_back(pair.second);
  }

  json = wpi::json{{"field",
                    {{"length", layout.m_fieldLength.value()},
                     {"width", layout.m_fieldWidth.value()}}},
                   {"tags", tagVector}};
}

void frc::from_json(const wpi::json& json, AprilTagFieldLayout& layout) {
  layout.m_apriltags.clear();
  for (const auto& tag : json.at("tags").get<std::vector<AprilTag>>()) {
    layout.m_apriltags[tag.ID] = tag;
  }

  layout.m_fieldLength =
      units::meter_t{json.at("field").at("length").get<double>()};
  layout.m_fieldWidth =
      units::meter_t{json.at("field").at("width").get<double>()};
}
