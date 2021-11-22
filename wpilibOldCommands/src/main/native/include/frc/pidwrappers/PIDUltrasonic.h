// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PIDSource.h"
#include "frc/Ultrasonic.h"

namespace frc {

class PIDUltrasonic : public PIDSource, public Ultrasonic {
  using Ultrasonic::Ultrasonic;

 public:
  /**
   * Get the range for the PIDSource base object.
   *
   * @return The range
   */
  double PIDGet() override;
};

}  // namespace frc
