// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/fields/FieldImage.hpp"
#include "wpi/fields/FieldTag.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::fields {

enum class FieldId;

class Field;

WPILIB_DLLEXPORT
Field GetField(FieldId field);

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const Field& field);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, Field& field);

/** A FIRST field with optional field image metadata and optional tag poses. */
class WPILIB_DLLEXPORT Field final {
 public:
  /** Common origin positions for the field tag coordinate system. */
  enum class OriginPosition {
    /** Blue alliance wall, right side. */
    BLUE_ALLIANCE_WALL_RIGHT_SIDE,
    /** Red alliance wall, right side. */
    RED_ALLIANCE_WALL_RIGHT_SIDE,
  };

  /** Constructs an empty field object. */
  Field() = default;

  /**
   * Constructs a field with values imported from a JSON file.
   *
   * @param path Path of the JSON file to import from.
   */
  explicit Field(std::string_view path);

  /**
   * Constructs a field.
   *
   * @param name Human-readable field name.
   * @param season Field season.
   * @param game Game name.
   * @param image Field image metadata, or std::nullopt if this field has no
   *              image.
   * @param fieldLength Field length.
   * @param fieldWidth Field width.
   * @param program FIRST program.
   * @param tags Field tag metadata.
   */
  Field(std::string_view name, std::string_view season, std::string_view game,
        std::optional<FieldImage> image, wpi::units::meters<> fieldLength,
        wpi::units::meters<> fieldWidth, std::string_view program,
        std::vector<FieldTag> tags = {});

  Field(const Field& other);

  Field& operator=(const Field& other);

  Field(Field&& other) noexcept;

  Field& operator=(Field&& other) noexcept;

  /**
   * Returns the human-readable field name.
   *
   * @return Human-readable field name.
   */
  std::string_view GetName() const;

  /**
   * Returns the field season.
   *
   * @return Field season.
   */
  std::string_view GetSeason() const;

  /**
   * Returns the game name.
   *
   * @return Game name.
   */
  std::string_view GetGame() const;

  /**
   * Returns true if this field has field image metadata.
   *
   * @return True if this field has field image metadata.
   */
  bool HasImage() const;

  /**
   * Returns the field image metadata.
   *
   * @return Field image metadata, or std::nullopt if this field has no image.
   */
  std::optional<FieldImage> GetImage() const;

  /**
   * Gets the length.
   *
   * @return Length.
   */
  wpi::units::meters<> GetLength() const;

  /**
   * Gets the width.
   *
   * @return Width.
   */
  wpi::units::meters<> GetWidth() const;

  /**
   * Returns the FIRST program.
   *
   * @return FIRST program.
   */
  std::string_view GetProgram() const;

  /**
   * Returns the JSON resource path for this field relative to
   * BASE_RESOURCE_DIR.
   *
   * @return JSON resource path, or an empty string if none exists.
   */
  std::string_view GetResourceFile() const;

  /**
   * Returns the original JSON contents for this field.
   *
   * @return Original JSON contents, or an empty string if none were retained.
   */
  std::string_view GetJson() const;

  /**
   * Returns true if this field has tag metadata.
   *
   * @return True if this field has tag metadata.
   */
  bool HasTags() const;

  /**
   * Returns a vector of all the FieldTags used by this field.
   *
   * @return List of tags.
   */
  std::vector<FieldTag> GetTags() const;

  /**
   * Sets the origin based on a predefined enumeration of coordinate frame
   * origins. The origins are calculated from the field dimensions.
   *
   * This transforms the Pose3ds returned by GetTagPose(int) to return the
   * correct pose relative to a predefined coordinate frame.
   *
   * @param origin The predefined origin.
   */
  void SetOrigin(OriginPosition origin);

  /**
   * Sets the origin for tag pose transformation.
   *
   * This transforms the Pose3ds returned by GetTagPose(int) to return the
   * correct pose relative to the provided origin.
   *
   * @param origin The new origin for tag transformations.
   */
  void SetOrigin(const wpi::math::Pose3d& origin);

  /**
   * Returns the origin used for tag pose transformation.
   *
   * @return The origin.
   */
  wpi::math::Pose3d GetOrigin() const;

  /**
   * Gets a tag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the ID that was passed in or an empty
   * optional if a tag with that ID is not found.
   */
  std::optional<wpi::math::Pose3d> GetTagPose(int ID) const;

  /**
   * Serializes a field to a JSON file.
   *
   * @param path The path to write the JSON file to.
   */
  void Serialize(std::string_view path) const;

  /**
   * Checks equality between this Field and another object.
   */
  bool operator==(const Field& other) const;

 private:
  void CopyFrom(const Field& other);

  void MoveFrom(Field&& other);

  void SetOwnedTags(std::vector<FieldTag> tags);

  std::string m_name;
  std::string m_season;
  std::string m_game;
  std::optional<FieldImage> m_image;
  wpi::units::meters<> m_fieldLength;
  wpi::units::meters<> m_fieldWidth;
  std::string m_program;
  std::string m_resourceFile;
  bool m_hasTags = false;
  std::span<const FieldTag> m_tags;
  std::vector<FieldTag> m_ownedTags;
  wpi::math::Pose3d m_origin;
  std::string_view (*m_getJson)() = nullptr;
  std::string m_json;

  friend Field GetField(FieldId field);
  friend void to_json(wpi::util::json& json, const Field& field);
  friend void from_json(const wpi::util::json& json, Field& field);
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const Field& field);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, Field& field);

}  // namespace wpi::fields
