// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/servo/LinearServo.h"

namespace frc {

/**
 * Actuonix L16-R servo.
 *
 * <p>This uses the following bounds for PWM values.
 * https://actuonix-com.3dcartstores.com/assets/images/datasheets/ActuonixL16Datasheet.pdf
 *
 * <ul>
 *   <li>2.0ms = full extend
 *   <li>1.0ms = full retract
 * </ul>
 */
class ActuonixL16R : public LinearServo {
 public:
  explicit ActuonixL16R(int channel);

  ActuonixL16R(ActuonixL16R&&) = default;
  ActuonixL16R& operator=(ActuonixL16R&&) = default;
};

}  // namespace frc
