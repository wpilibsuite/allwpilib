// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <optional>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/math/geometry/Rotation2d.hpp"

namespace wpi {

/**
 * A controller POV direction.
 */
enum class POVDirection : uint8_t {
  /// POV center.
  CENTER = HAL_JOYSTICK_POV_CENTERED,
  /// POV up.
  UP = HAL_JOYSTICK_POV_UP,
  /// POV up right.
  UP_RIGHT = HAL_JOYSTICK_POV_RIGHT_UP,
  /// POV right.
  RIGHT = HAL_JOYSTICK_POV_RIGHT,
  /// POV down right.
  DOWN_RIGHT = HAL_JOYSTICK_POV_RIGHT_DOWN,
  /// POV down.
  DOWN = HAL_JOYSTICK_POV_DOWN,
  /// POV down left.
  DOWN_LEFT = HAL_JOYSTICK_POV_LEFT_DOWN,
  /// POV left.
  LEFT = HAL_JOYSTICK_POV_LEFT,
  /// POV up left.
  UP_LEFT = HAL_JOYSTICK_POV_LEFT_UP,
};

/**
 * Gets the angle of a POVDirection.
 *
 * @param angle The POVDirection to convert.
 * @return The angle clockwise from straight up, or std::nullopt if the
 * POVDirection is CENTER.
 */
constexpr std::optional<wpi::math::Rotation2d> GetPOVAngle(POVDirection angle) {
  using enum POVDirection;
  switch (angle) {
    case CENTER:
      return std::nullopt;
    case UP:
      return wpi::math::Rotation2d{0_deg};
    case UP_RIGHT:
      return wpi::math::Rotation2d{45_deg};
    case RIGHT:
      return wpi::math::Rotation2d{90_deg};
    case DOWN_RIGHT:
      return wpi::math::Rotation2d{135_deg};
    case DOWN:
      return wpi::math::Rotation2d{180_deg};
    case DOWN_LEFT:
      return wpi::math::Rotation2d{225_deg};
    case LEFT:
      return wpi::math::Rotation2d{270_deg};
    case UP_LEFT:
      return wpi::math::Rotation2d{315_deg};
    default:
      return std::nullopt;
  }
}

}  // namespace wpi
