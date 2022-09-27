// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/DriverStation.h"
#include "frc/apriltag/AprilTag.h"
#include "frc/geometry/Pose3d.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {
/**
 * Class for representing a layout of AprilTags on a field and reading them from
 * a JSON format.
 *
 * The JSON format contains a top-level "tags" field, which is a list of all
 * AprilTags contained within a layout. Each AprilTag serializes to a JSON
 * object containing an ID and a Pose3d.
 *
 * Pose3ds are assumed to be measured from the bottom-left corner of the field,
 * when the blue alliance is at the left. Pose3ds will automatically be returned
 * as passed in when calling GetTagPose(int).  Setting an alliance color via
 * SetAlliance(DriverStation::Alliance) will mirror the poses returned from
 * GetTagPose(int) to be correct relative to the other alliance.
 */
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
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
   */
  explicit AprilTagFieldLayout(std::vector<AprilTag>& apriltags);

  /**
   * Set the alliance that your team is on.
   *
   * This changes the GetTagPose(int) method to return the correct pose for your
   * alliance.
   *
   * @param alliance The alliance to mirror poses for.
   */
  void SetAlliance(DriverStation::Alliance alliance);

  /**
   * Gets an AprilTag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the id that was passed in.
   */
  Pose3d GetTagPose(int ID) const;

  /**
   * Serializes an AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write the JSON file to.
   */
  void Serialize(std::string_view path);

  /*
   * Checks equality between this AprilTagFieldLayout and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const AprilTagFieldLayout& other) const;

  /**
   * Checks inequality between this AprilTagFieldLayout and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are not equal.
   */
  bool operator!=(const AprilTagFieldLayout& other) const;

 private:
  std::vector<AprilTag> m_apriltags;
  bool m_mirror = false;

  friend WPILIB_DLLEXPORT void to_json(wpi::json& json,
                                       const AprilTagFieldLayout& layout);

  friend WPILIB_DLLEXPORT void from_json(const wpi::json& json,
                                         AprilTagFieldLayout& layout);
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTagFieldLayout& layout);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTagFieldLayout& layout);

}  // namespace frc
