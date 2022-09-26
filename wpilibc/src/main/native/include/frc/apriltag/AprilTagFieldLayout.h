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
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
  AprilTagFieldLayout() = default;

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   */
  explicit AprilTagFieldLayout(std::string_view path);

  /**
   * Construct a new AprilTagFieldLayout from a vector of AprilTag objects.
   */
  explicit AprilTagFieldLayout(const std::vector<AprilTag>& apriltags);

  /**
   * Set the alliance that your team is on.
   *
   * This changes the AprilTagFieldLayout::getTag(int) method to return the
   * correct pose for your alliance.
   *
   * @param alliance the alliance to mirror poses for
   */
  void SetAlliance(DriverStation::Alliance alliance);

  /**
   * Gets an AprilTag pose by its id.
   *
   * @param id the id of the tag
   * @return The pose corresponding to the id that was passed in.
   */
  Pose3d GetTagPose(int id) const;

  /**
   * Serializes an AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write to
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
