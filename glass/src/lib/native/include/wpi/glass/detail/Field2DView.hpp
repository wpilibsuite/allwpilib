// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

#include <imgui.h>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"

namespace wpi::glass::detail {

// Per-frame field data (not persistent). Image bounds may include margins
// outside the playable field bounds, min and max.
struct FieldFrameData {
  /**
   * Converts a screen position to a field position, clamping to field bounds.
   *
   * @param cursor Screen position.
   * @return Position in meters relative to field center.
   */
  wpi::math::Translation2d GetPosFromScreen(const ImVec2& cursor) const {
    return {
        wpi::units::meter_t{
            (std::clamp(cursor.x, min.x, max.x) - (min.x + max.x) / 2) / scale},
        wpi::units::meter_t{
            ((min.y + max.y) / 2 - std::clamp(cursor.y, min.y, max.y)) /
            scale}};
  }

  /**
   * Converts a field position to a screen position.
   *
   * @param pos Position in meters relative to field center.
   * @return Screen position, with +X right and +Y up.
   */
  ImVec2 GetScreenFromPos(const wpi::math::Translation2d& pos) const {
    return {(min.x + max.x) / 2 + scale * pos.X().to<float>(),
            (min.y + max.y) / 2 - scale * pos.Y().to<float>()};
  }

  /**
   * Converts a field heading to screen rotation components.
   *
   * @param rotation Field heading.
   * @return Cosine and sine of the screen rotation.
   */
  ImVec2 GetScreenRotation(const wpi::math::Rotation2d& rotation) const {
    return {static_cast<float>(rotation.Cos()),
            -static_cast<float>(rotation.Sin())};
  }

  // Screen coordinates.
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // Scaling from meters to screen units.
};

}  // namespace wpi::glass::detail
