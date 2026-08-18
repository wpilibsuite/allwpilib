// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/SimDevice.hpp"
#include "wpi/units/angle.hpp"

namespace wpi::romi {

/**
 * @ingroup romi_api
 * @{
 */

/**
 * RomiServo.
 *
 * <p>A SimDevice based servo
 */
class RomiServo {
 public:
  /**
   * Constructs a RomiServo.
   *
   * @param channel the servo channel
   */
  explicit RomiServo(int channel);

  /**
   * Set the servo angle.
   *
   * @param angle Desired angle in radians
   */
  void SetAngle(wpi::units::radian_t angle);

  /**
   * Get the servo angle.
   *
   * @return Current servo angle in radians
   */
  wpi::units::radian_t GetAngle() const;

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
};

/** @} */

}  // namespace wpi::romi
