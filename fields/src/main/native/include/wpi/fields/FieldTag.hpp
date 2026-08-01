// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::fields {

/**
 * Represents a field tag's metadata.
 */
struct WPILIB_DLLEXPORT FieldTag {
  /// The tag's ID.
  int ID;

  /// The tag's pose.
  wpi::math::Pose3d pose;

  bool operator==(const FieldTag&) const = default;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const FieldTag& tag);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, FieldTag& tag);

}  // namespace wpi::fields
