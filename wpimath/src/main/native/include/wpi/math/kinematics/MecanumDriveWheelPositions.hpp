// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/length.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel positions for a mecanum drive drivetrain.
 */
struct WPILIB_DLLEXPORT MecanumDriveWheelPositions {
  /**
   * Distance driven by the front-left wheel.
   */
  wpi::units::meter_t frontLeft = 0_m;

  /**
   * Distance driven by the front-right wheel.
   */
  wpi::units::meter_t frontRight = 0_m;

  /**
   * Distance driven by the rear-left wheel.
   */
  wpi::units::meter_t rearLeft = 0_m;

  /**
   * Distance driven by the rear-right wheel.
   */
  wpi::units::meter_t rearRight = 0_m;

  /**
   * Checks equality between this MecanumDriveWheelPositions and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const MecanumDriveWheelPositions& other) const =
      default;

  constexpr MecanumDriveWheelPositions Interpolate(
      const MecanumDriveWheelPositions& endValue, double t) const {
    return {wpi::util::Lerp(frontLeft, endValue.frontLeft, t),
            wpi::util::Lerp(frontRight, endValue.frontRight, t),
            wpi::util::Lerp(rearLeft, endValue.rearLeft, t),
            wpi::util::Lerp(rearRight, endValue.rearRight, t)};
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/MecanumDriveWheelPositionsProto.hpp"
#include "wpi/math/kinematics/struct/MecanumDriveWheelPositionsStruct.hpp"
