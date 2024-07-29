// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <units/length.h>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/apriltag/AprilTag.h"
#include "frc/apriltag/AprilTagFields.h"
#include "frc/geometry/Pose3d.h"

namespace frc {
/**
 * Class for representing a layout of AprilTags on a field and reading them from
 * a JSON format.
 *
 * The JSON format contains two top-level objects, "tags" and "field".
 * The "tags" object is a list of all AprilTags contained within a layout. Each
 * AprilTag serializes to a JSON object containing an ID and a Pose3d. The
 * "field" object is a descriptor of the size of the field in meters with
 * "width" and "length" values.  This is to account for arbitrary field sizes
 * when transforming the poses.
 *
 * Pose3ds in the JSON are measured using the normal FRC coordinate system, NWU
 * with the origin at the bottom-right corner of the blue alliance wall.
 * SetOrigin(OriginPosition) can be used to change the poses returned from
 * GetTagPose(int) to be from the perspective of a specific alliance.
 *
 * Tag poses represent the center of the tag, with a zero rotation representing
 * a tag that is upright and facing away from the (blue) alliance wall (that is,
 * towards the opposing alliance). */
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
  /**
   * Common origin positions for the AprilTag coordinate system.
   */
  enum class OriginPosition {
    /// Blue alliance wall, right side.
    kBlueAllianceWallRightSide,
    /// Red alliance wall, right side.
    kRedAllianceWallRightSide,
  };

  /**
   * Loads an AprilTagFieldLayout from a predefined field
   *
   * @param field The predefined field
   * @return AprilTagFieldLayout of the field
   */
  static AprilTagFieldLayout LoadField(AprilTagField field);

  AprilTagFieldLayout() = default;

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   *
   * @param path Path of the JSON file to import from.
   */
  explicit AprilTagFieldLayout(std::string_view path);

  /**
   * Construct a new AprilTagFieldLayout from a vector of AprilTag objects.
   *
   * @param apriltags Vector of AprilTags.
   * @param fieldLength Length of field the layout is representing.
   * @param fieldWidth Width of field the layout is representing.
   */
  AprilTagFieldLayout(std::vector<AprilTag> apriltags,
                      units::meter_t fieldLength, units::meter_t fieldWidth);

  /**
   * Returns the length of the field the layout is representing.
   * @return length
   */
  units::meter_t GetFieldLength() const;

  /**
   * Returns the length of the field the layout is representing.
   * @return width
   */
  units::meter_t GetFieldWidth() const;

  /**
   * Returns a vector of all the april tags used in this layout.
   * @return list of tags
   */
  std::vector<AprilTag> GetTags() const;

  /**
   * Sets the origin based on a predefined enumeration of coordinate frame
   * origins. The origins are calculated from the field dimensions.
   *
   * This transforms the Pose3ds returned by GetTagPose(int) to return the
   * correct pose relative to a predefined coordinate frame.
   *
   * @param origin The predefined origin
   */
  void SetOrigin(OriginPosition origin);

  /**
   * Sets the origin for tag pose transformation.
   *
   * This transforms the Pose3ds returned by GetTagPose(int) to return the
   * correct pose relative to the provided origin.
   *
   * @param origin The new origin for tag transformations
   */
  void SetOrigin(const Pose3d& origin);

  /**
   * Returns the origin used for tag pose transformation.
   * @return the origin
   */
  Pose3d GetOrigin() const;

  /**
   * Gets an AprilTag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the ID that was passed in or an empty
   * optional if a tag with that ID is not found.
   */
  std::optional<Pose3d> GetTagPose(int ID) const;

  /**
   * Serializes an AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write the JSON file to.
   */
  void Serialize(std::string_view path);

  /*
   * Checks equality between this AprilTagFieldLayout and another object.
   */
  bool operator==(const AprilTagFieldLayout&) const = default;

 private:
  std::unordered_map<int, AprilTag> m_apriltags;
  units::meter_t m_fieldLength;
  units::meter_t m_fieldWidth;
  Pose3d m_origin;

  friend WPILIB_DLLEXPORT void to_json(wpi::json& json,
                                       const AprilTagFieldLayout& layout);

  friend WPILIB_DLLEXPORT void from_json(const wpi::json& json,
                                         AprilTagFieldLayout& layout);
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTagFieldLayout& layout);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTagFieldLayout& layout);

/**
 * Loads an AprilTagFieldLayout from a predefined field
 *
 * @param field The predefined field
 * @return AprilTagFieldLayout of the field
 * @deprecated Use AprilTagFieldLayout::LoadField() instead
 */
[[deprecated("Use AprilTagFieldLayout::LoadField() instead")]]
WPILIB_DLLEXPORT AprilTagFieldLayout
LoadAprilTagLayoutField(AprilTagField field);

}  // namespace frc
