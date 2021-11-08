// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/AnalogInput.h"
#include "frc/PIDSource.h"

namespace frc {

class PIDAnalogInput : public PIDSource, public AnalogInput {
 public:
  /**
   * Get the Average value for the PID Source base object.
   *
   * @return The average voltage.
   */
  double PIDGet() override;
};

}  // namespace frc
