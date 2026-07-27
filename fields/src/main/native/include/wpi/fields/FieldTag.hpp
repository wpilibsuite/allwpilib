// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::fields {

/**
 * Represents a field tag's metadata.
 */
struct FieldTag {
  /// The tag's ID.
  int ID;

  /// The tag's pose.
  wpi::math::Pose3d pose;

  bool operator==(const FieldTag&) const = default;
};

void to_json(wpi::util::json& json, const FieldTag& tag);

void from_json(const wpi::util::json& json, FieldTag& tag);

}  // namespace wpi::fields
