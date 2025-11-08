// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/RawFrame.h"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/json_fwd.hpp"

#include "wpi/math/geometry/Pose3d.hpp"

namespace frc {

/**
 * Represents an AprilTag's metadata.
 */
struct WPILIB_DLLEXPORT AprilTag {
  /// The tag's ID.
  int ID;

  /// The tag's pose.
  Pose3d pose;

  bool operator==(const AprilTag&) const = default;

  static bool Generate36h11AprilTagImage(wpi::RawFrame* frame, int id);
  static bool Generate16h5AprilTagImage(wpi::RawFrame* frame, int id);
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTag& apriltag);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTag& apriltag);

}  // namespace frc
