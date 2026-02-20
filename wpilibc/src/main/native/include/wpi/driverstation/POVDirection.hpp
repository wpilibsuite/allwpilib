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
  kCenter = HAL_JoystickPOV_kCentered,
  /// POV up.
  kUp = HAL_JoystickPOV_kUp,
  /// POV up right.
  kUpRight = HAL_JoystickPOV_kRightUp,
  /// POV right.
  kRight = HAL_JoystickPOV_kRight,
  /// POV down right.
  kDownRight = HAL_JoystickPOV_kRightDown,
  /// POV down.
  kDown = HAL_JoystickPOV_kDown,
  /// POV down left.
  kDownLeft = HAL_JoystickPOV_kLeftDown,
  /// POV left.
  kLeft = HAL_JoystickPOV_kLeft,
  /// POV up left.
  kUpLeft = HAL_JoystickPOV_kLeftUp,
};

/**
 * Gets the angle of a POVDirection.
 *
 * @param angle The POVDirection to convert.
 * @return The angle clockwise from straight up, or std::nullopt if the
 * POVDirection is kCenter.
 */
constexpr std::optional<wpi::math::Rotation2d> GetPOVAngle(POVDirection angle) {
  using enum POVDirection;
  switch (angle) {
    case kCenter:
      return std::nullopt;
    case kUp:
      return wpi::math::Rotation2d{0_deg};
    case kUpRight:
      return wpi::math::Rotation2d{45_deg};
    case kRight:
      return wpi::math::Rotation2d{90_deg};
    case kDownRight:
      return wpi::math::Rotation2d{135_deg};
    case kDown:
      return wpi::math::Rotation2d{180_deg};
    case kDownLeft:
      return wpi::math::Rotation2d{225_deg};
    case kLeft:
      return wpi::math::Rotation2d{270_deg};
    case kUpLeft:
      return wpi::math::Rotation2d{315_deg};
    default:
      return std::nullopt;
  }
}

}  // namespace wpi
