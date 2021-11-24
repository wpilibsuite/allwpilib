// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/AnalogPotentiometer.h"
#include "frc/PIDSource.h"

namespace frc {

/**
 * Wrapper so that PIDSource is implemented for AnalogPotentiometer for old
 * PIDController
 */
class PIDAnalogPotentiometer : public PIDSource, public AnalogPotentiometer {
  using AnalogPotentiometer::AnalogPotentiometer;

 public:
  /**
   * Implement the PIDSource interface.
   *
   * @return The current reading.
   */
  double PIDGet() override;
};

}  // namespace frc
