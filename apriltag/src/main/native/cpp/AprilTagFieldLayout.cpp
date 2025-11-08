// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTagFieldLayout.hpp"

#include <system_error>
#include <utility>
#include <vector>

#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi::apriltag;

AprilTagFieldLayout::AprilTagFieldLayout(std::string_view path) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(path);
  if (!fileBuffer) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::util::json json = wpi::util::json::parse(fileBuffer.value()->GetCharBuffer());

  for (const auto& tag : json.at("tags").get<std::vector<AprilTag>>()) {
    m_apriltags[tag.ID] = tag;
  }
  m_fieldWidth = wpi::units::meter_t{json.at("field").at("width").get<double>()};
  m_fieldLength = wpi::units::meter_t{json.at("field").at("length").get<double>()};
}

AprilTagFieldLayout::AprilTagFieldLayout(std::vector<AprilTag> apriltags,
                                         wpi::units::meter_t fieldLength,
                                         wpi::units::meter_t fieldWidth)
    : m_fieldLength(std::move(fieldLength)),
      m_fieldWidth(std::move(fieldWidth)) {
  for (const auto& tag : apriltags) {
    m_apriltags[tag.ID] = tag;
  }
}

wpi::units::meter_t AprilTagFieldLayout::GetFieldLength() const {
  return m_fieldLength;
}

wpi::units::meter_t AprilTagFieldLayout::GetFieldWidth() const {
  return m_fieldWidth;
}

std::vector<AprilTag> AprilTagFieldLayout::GetTags() const {
  std::vector<AprilTag> tags;
  tags.reserve(m_apriltags.size());
  for (const auto& tag : m_apriltags) {
    tags.emplace_back(tag.second);
  }
  return tags;
}

void AprilTagFieldLayout::SetOrigin(OriginPosition origin) {
  switch (origin) {
    case OriginPosition::kBlueAllianceWallRightSide:
      SetOrigin(wpi::math::Pose3d{});
      break;
    case OriginPosition::kRedAllianceWallRightSide:
      SetOrigin(wpi::math::Pose3d{wpi::math::Translation3d{m_fieldLength, m_fieldWidth, 0_m},
                       wpi::math::Rotation3d{0_deg, 0_deg, 180_deg}});
      break;
    default:
      throw std::invalid_argument("Invalid origin");
  }
}

void AprilTagFieldLayout::SetOrigin(const wpi::math::Pose3d& origin) {
  m_origin = origin;
}

wpi::math::Pose3d AprilTagFieldLayout::GetOrigin() const {
  return m_origin;
}

std::optional<wpi::math::Pose3d> AprilTagFieldLayout::GetTagPose(int ID) const {
  const auto& it = m_apriltags.find(ID);
  if (it == m_apriltags.end()) {
    return std::nullopt;
  }
  return it->second.pose.RelativeTo(m_origin);
}

void AprilTagFieldLayout::Serialize(std::string_view path) {
  std::error_code error_code;

  wpi::util::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(fmt::format("Cannot open file: {}", path));
  }

  wpi::util::json json = *this;
  output << json;
  output.flush();
}

void wpi::apriltag::to_json(wpi::util::json& json, const AprilTagFieldLayout& layout) {
  std::vector<AprilTag> tagVector;
  tagVector.reserve(layout.m_apriltags.size());
  for (const auto& pair : layout.m_apriltags) {
    tagVector.push_back(pair.second);
  }

  json = wpi::util::json{{"field",
                    {{"length", layout.m_fieldLength.value()},
                     {"width", layout.m_fieldWidth.value()}}},
                   {"tags", tagVector}};
}

void wpi::apriltag::from_json(const wpi::util::json& json, AprilTagFieldLayout& layout) {
  layout.m_apriltags.clear();
  for (const auto& tag : json.at("tags").get<std::vector<AprilTag>>()) {
    layout.m_apriltags[tag.ID] = tag;
  }

  layout.m_fieldLength =
      wpi::units::meter_t{json.at("field").at("length").get<double>()};
  layout.m_fieldWidth =
      wpi::units::meter_t{json.at("field").at("width").get<double>()};
}

// Use namespace declaration for forward declaration
namespace wpi::apriltag {

// C++ generated from resource files
std::string_view GetResource_2022_rapidreact_json();
std::string_view GetResource_2023_chargedup_json();
std::string_view GetResource_2024_crescendo_json();
std::string_view GetResource_2025_reefscape_welded_json();
std::string_view GetResource_2025_reefscape_andymark_json();

}  // namespace wpi::apriltag

AprilTagFieldLayout AprilTagFieldLayout::LoadField(AprilTagField field) {
  std::string_view fieldString;
  switch (field) {
    case AprilTagField::k2022RapidReact:
      fieldString = GetResource_2022_rapidreact_json();
      break;
    case AprilTagField::k2023ChargedUp:
      fieldString = GetResource_2023_chargedup_json();
      break;
    case AprilTagField::k2024Crescendo:
      fieldString = GetResource_2024_crescendo_json();
      break;
    case AprilTagField::k2025ReefscapeWelded:
      fieldString = GetResource_2025_reefscape_welded_json();
      break;
    case AprilTagField::k2025ReefscapeAndyMark:
      fieldString = GetResource_2025_reefscape_andymark_json();
      break;
    case AprilTagField::kNumFields:
      throw std::invalid_argument("Invalid Field");
  }

  wpi::util::json json = wpi::util::json::parse(fieldString);
  return json.get<AprilTagFieldLayout>();
}

AprilTagFieldLayout wpi::apriltag::LoadAprilTagLayoutField(AprilTagField field) {
  return AprilTagFieldLayout::LoadField(field);
}
