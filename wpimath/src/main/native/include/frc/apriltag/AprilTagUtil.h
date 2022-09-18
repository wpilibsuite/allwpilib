// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>
#include "frc/geometry/Pose3d.h"

#include <wpi/SymbolExports.h>

namespace frc {
class WPILIB_DLLEXPORT AprilTagUtil {
 public:
  AprilTagUtil() = delete;

  struct WPILIB_DLLEXPORT AprilTag {
      int id;

      Pose3d pose;
      
      /**
       * Checks equality between this State and another object.
       *
       * @param other The other object.
       * @return Whether the two objects are equal.
       */
      bool operator==(const AprilTag& other) const;

      /**
       * Checks inequality between this State and another object.
       *
       * @param other The other object.
       * @return Whether the two objects are not equal.
       */
      bool operator!=(const AprilTag& other) const;
  };

  /**
   * Exports a AprilTag Layout JSON file.
   *
   * @param apriltag the AprilTag layout to export
   * @param path the path of the file to export to
   */
  static void ToJson(const std::vector<AprilTag>& apriltagFieldLayout,
                               std::string_view path);
  /**
   * Imports a AprilTag Layout JSON file.
   *
   * @param path The path of the json file to import from.
   *
   * @return The AprilTag layout represented by the file.
   */
  static std::vector<AprilTag> FromJson(std::string_view path);

  /**
   * Deserializes a AprilTag layout JSON.
   *
   * @param apriltag the AprilTag layout to export
   *
   * @return the string containing the serialized JSON
   */
  static std::string SerializeAprilTagLayout(const std::vector<AprilTag>& apriltagFieldLayout);

  /**
   * Serializes a AprilTag layout JSON.
   *
   * @param jsonStr the string containing the serialized JSON
   *
   * @return the AprilTag layout represented by the JSON
   */
  static std::vector<AprilTag> DeserializeAprilTagLayout(std::string_view jsonStr);
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTagUtil::AprilTag& apriltag);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTagUtil::AprilTag& apriltag);

}  // namespace frc
