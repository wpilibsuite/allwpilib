// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/AnalogInput.h"
#include "frc/PIDSource.h"

namespace frc {

/**
 * Wrapper so that PIDSource is implemented for AnalogInput for old
 * PIDController
 */
class PIDAnalogInput : public PIDSource, public AnalogInput {
  using AnalogInput::AnalogInput;

 public:
  /**
   * Get the Average value for the PID Source base object.
   *
   * @return The average voltage.
   */
  double PIDGet() override;
};

}  // namespace frc
