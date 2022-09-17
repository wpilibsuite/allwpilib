// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/SymbolExports.h>

#include "frc/apriltag/AprilTagFieldLayout.h"

namespace frc {
class WPILIB_DLLEXPORT AprilTagUtil {
 public:
  AprilTagUtil() = delete;

  /**
   * Exports a AprilTag Layout JSON file.
   *
   * @param apriltag the AprilTag layout to export
   * @param path the path of the file to export to
   */
  static void ToJson(const AprilTagFieldLayout& apriltagFieldLayout,
                               std::string_view path);
  /**
   * Imports a AprilTag Layout JSON file.
   *
   * @param path The path of the json file to import from.
   *
   * @return The AprilTag layout represented by the file.
   */
  static AprilTagFieldLayout FromJson(std::string_view path);

  /**
   * Deserializes a AprilTag layout JSON.
   *
   * @param apriltag the AprilTag layout to export
   *
   * @return the string containing the serialized JSON
   */
  static std::string SerializeAprilTagLayout(const AprilTagFieldLayout& apriltagFieldLayout);

  /**
   * Serializes a AprilTag layout JSON.
   *
   * @param jsonStr the string containing the serialized JSON
   *
   * @return the AprilTag layout represented by the JSON
   */
  static AprilTagFieldLayout DeserializeAprilTagLayout(std::string_view jsonStr);
};
}  // namespace frc
