// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/Field.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi::fields;

Field::Field(std::string_view path) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(path);
  if (!fileBuffer) {
    throw std::runtime_error(std::format("Cannot open file: {}", path));
  }

  auto buf = fileBuffer.value()->GetCharBuffer();
  m_json.assign(buf.begin(), buf.end());
  auto json = wpi::util::json::parse_or_throw(m_json);
  *this = json.get<Field>();
  m_json.assign(buf.begin(), buf.end());
}

Field::Field(std::string_view name, std::string_view season,
             std::string_view game, std::optional<FieldImage> image,
             wpi::units::meters<> fieldLength, wpi::units::meters<> fieldWidth,
             std::string_view program, std::vector<FieldTag> tags)
    : m_name{name},
      m_season{season},
      m_game{game},
      m_image{std::move(image)},
      m_fieldLength{std::move(fieldLength)},
      m_fieldWidth{std::move(fieldWidth)},
      m_program{program},
      m_hasTags{!tags.empty()} {
  SetOwnedTags(std::move(tags));
}

Field::Field(const Field& other) {
  CopyFrom(other);
}

Field& Field::operator=(const Field& other) {
  if (this != &other) {
    CopyFrom(other);
  }
  return *this;
}

Field::Field(Field&& other) noexcept {
  MoveFrom(std::move(other));
}

Field& Field::operator=(Field&& other) noexcept {
  if (this != &other) {
    MoveFrom(std::move(other));
  }
  return *this;
}

std::string_view Field::GetName() const {
  return m_name;
}

std::string_view Field::GetSeason() const {
  return m_season;
}

std::string_view Field::GetGame() const {
  return m_game;
}

bool Field::HasImage() const {
  return m_image.has_value();
}

std::optional<FieldImage> Field::GetImage() const {
  return m_image;
}

wpi::units::meters<> Field::GetLength() const {
  return m_fieldLength;
}

wpi::units::meters<> Field::GetWidth() const {
  return m_fieldWidth;
}

std::string_view Field::GetProgram() const {
  return m_program;
}

std::string_view Field::GetResourceFile() const {
  return m_resourceFile;
}

std::string_view Field::GetJson() const {
  if (!m_json.empty()) {
    return m_json;
  }
  if (m_getJson) {
    return m_getJson();
  }
  return {};
}

bool Field::HasTags() const {
  return m_hasTags;
}

std::vector<FieldTag> Field::GetTags() const {
  return {m_tags.begin(), m_tags.end()};
}

void Field::SetOrigin(OriginPosition origin) {
  switch (origin) {
    case OriginPosition::BLUE_ALLIANCE_WALL_RIGHT_SIDE:
      SetOrigin(wpi::math::Pose3d{});
      break;
    case OriginPosition::RED_ALLIANCE_WALL_RIGHT_SIDE:
      SetOrigin(wpi::math::Pose3d{
          wpi::math::Translation3d{m_fieldLength, m_fieldWidth, 0_m},
          wpi::math::Rotation3d{0_deg, 0_deg, 180_deg}});
      break;
    default:
      throw std::invalid_argument("Invalid origin");
  }
}

void Field::SetOrigin(const wpi::math::Pose3d& origin) {
  m_origin = origin;
}

wpi::math::Pose3d Field::GetOrigin() const {
  return m_origin;
}

std::optional<wpi::math::Pose3d> Field::GetTagPose(int ID) const {
  for (const auto& tag : m_tags) {
    if (tag.ID == ID) {
      return tag.pose.RelativeTo(m_origin);
    }
  }
  return std::nullopt;
}

void Field::Serialize(std::string_view path) const {
  std::error_code error_code;

  wpi::util::raw_fd_ostream output{path, error_code};
  if (error_code) {
    throw std::runtime_error(std::format("Cannot open file: {}", path));
  }

  wpi::util::json json = *this;
  output << json;
  output.flush();
}

bool Field::operator==(const Field& other) const {
  return GetName() == other.GetName() && m_season == other.m_season &&
         m_game == other.m_game && m_image == other.m_image &&
         m_fieldLength == other.m_fieldLength &&
         m_fieldWidth == other.m_fieldWidth && m_program == other.m_program &&
         m_hasTags == other.m_hasTags &&
         std::ranges::equal(m_tags, other.m_tags) && m_origin == other.m_origin;
}

void Field::CopyFrom(const Field& other) {
  m_name = other.m_name;
  m_season = other.m_season;
  m_game = other.m_game;
  m_image = other.m_image;
  m_fieldLength = other.m_fieldLength;
  m_fieldWidth = other.m_fieldWidth;
  m_program = other.m_program;
  m_resourceFile = other.m_resourceFile;
  m_hasTags = other.m_hasTags;
  m_ownedTags = other.m_ownedTags;
  m_origin = other.m_origin;
  m_getJson = other.m_getJson;
  m_json = other.m_json;
  m_tags = m_ownedTags.empty() ? other.m_tags : std::span{m_ownedTags};
}

void Field::MoveFrom(Field&& other) {
  m_name = std::move(other.m_name);
  m_season = std::move(other.m_season);
  m_game = std::move(other.m_game);
  m_image = std::move(other.m_image);
  m_fieldLength = std::move(other.m_fieldLength);
  m_fieldWidth = std::move(other.m_fieldWidth);
  m_program = std::move(other.m_program);
  m_resourceFile = std::move(other.m_resourceFile);
  m_hasTags = other.m_hasTags;
  m_ownedTags = std::move(other.m_ownedTags);
  m_origin = std::move(other.m_origin);
  m_getJson = other.m_getJson;
  m_json = std::move(other.m_json);
  m_tags = m_ownedTags.empty() ? other.m_tags : std::span{m_ownedTags};

  other.m_hasTags = false;
  other.m_tags = {};
  other.m_ownedTags.clear();
}

void Field::SetOwnedTags(std::vector<FieldTag> tags) {
  m_ownedTags = std::move(tags);
  m_tags = m_ownedTags;
}

void wpi::fields::to_json(wpi::util::json& json, const Field& field) {
  auto dimensions =
      wpi::util::json::object("length", field.m_fieldLength.value(), "width",
                              field.m_fieldWidth.value());
  json = wpi::util::json::object(
      "name", field.m_name, "season", field.m_season, "game", field.m_game,
      "field-dimensions", std::move(dimensions), "program", field.m_program);
  if (field.m_image) {
    json["field-image"] = *field.m_image;
  }
  if (field.m_hasTags) {
    json["field-tags"] = field.GetTags();
  }
}

void wpi::fields::from_json(const wpi::util::json& json, Field& field) {
  field.m_name = json.at("name").get_string();
  field.m_season = json.at("season").get_string();
  field.m_game = json.at("game").get_string();
  field.m_image = std::nullopt;
  if (json.contains("field-image")) {
    field.m_image = json.at("field-image").get<FieldImage>();
  }

  field.m_fieldLength = wpi::units::meters<>{
      json.at("field-dimensions").at("length").get_number()};
  field.m_fieldWidth = wpi::units::meters<>{
      json.at("field-dimensions").at("width").get_number()};
  field.m_program = json.at("program").get_string();
  field.m_resourceFile.clear();
  field.m_hasTags = json.contains("field-tags");
  if (field.m_hasTags) {
    std::vector<FieldTag> tags;
    for (auto&& jtag : json.at("field-tags").get_array()) {
      tags.emplace_back(jtag.get<FieldTag>());
    }
    field.SetOwnedTags(std::move(tags));
  } else {
    field.SetOwnedTags({});
  }
  field.m_origin = {};
  field.m_getJson = nullptr;
  field.m_json.clear();
}
