// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose3d.h"

namespace frc {
class WPILIB_DLLEXPORT AprilTagUtil {
 public:
  AprilTagUtil() = delete;

  /**
   * Deserializes a AprilTag layout JSON.
   *
   * @param apriltagLayout the AprilTag layout to export
   *
   * @return the string containing the serialized JSON
   */
  static std::string SerializeAprilTagLayout(
      const std::vector<std::pair<int, Pose3d>>& apriltagLayout);

  /**
   * Serializes a AprilTag layout JSON.
   *
   * @param jsonStr the string containing the serialized JSON
   *
   * @return the AprilTag layout represented by the JSON
   */
  static std::vector<std::pair<int, Pose3d>> DeserializeAprilTagLayout(
      std::string_view jsonStr);
};
}  // namespace frc
