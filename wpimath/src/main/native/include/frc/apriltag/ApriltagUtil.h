// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/SymbolExports.h>

#include "frc/apriltag/ApriltagFieldLayout.h"

namespace frc {
class WPILIB_DLLEXPORT ApriltagUtil {
 public:
  ApriltagUtil() = delete;

  /**
   * Exports a Apriltag Layout JSON file.
   *
   * @param apriltag the Apriltag layout to export
   * @param path the path of the file to export to
   */
  static void ToJson(const ApriltagFieldLayout& apriltagFieldLayout,
                               std::string_view path);
  /**
   * Imports a Apriltag Layout JSON file.
   *
   * @param path The path of the json file to import from.
   *
   * @return The Apriltag layout represented by the file.
   */
  static ApriltagFieldLayout FromJson(std::string_view path);

  /**
   * Deserializes a Apriltag layout JSON.
   *
   * @param apriltag the Apriltag layout to export
   *
   * @return the string containing the serialized JSON
   */
  static std::string SerializeApriltagLayout(const ApriltagFieldLayout& apriltagFieldLayout);

  /**
   * Serializes a Apriltag layout JSON.
   *
   * @param jsonStr the string containing the serialized JSON
   *
   * @return the Apriltag layout represented by the JSON
   */
  static ApriltagFieldLayout DeserializeApriltagLayout(std::string_view jsonStr);
};
}  // namespace frc
