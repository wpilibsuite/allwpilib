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
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @deprecated Use frc2::PIDController class instead which doesn't require this
 * wrapper.
 */
class WPI_DEPRECATED("Use frc2::PIDController class instead.")
    PIDAnalogPotentiometer : public PIDSource,
                             public AnalogPotentiometer {
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
