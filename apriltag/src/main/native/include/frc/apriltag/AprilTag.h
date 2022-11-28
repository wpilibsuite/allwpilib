// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose3d.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {

struct WPILIB_DLLEXPORT AprilTag {
  int ID;

  Pose3d pose;

  /**
   * Checks equality between this AprilTag and another object.
   */
  bool operator==(const AprilTag&) const = default;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTag& apriltag);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTag& apriltag);

}  // namespace frc
