/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/PIDOutput.h"

namespace frc {

/**
 * Interface for speed controlling devices.
 */
class SpeedController : public PIDOutput {
 public:
  virtual ~SpeedController() = default;

  /**
   * Common interface for setting the speed of a speed controller.
   *
   * @param speed The speed to set.  Value should be between -1.0 and 1.0.
   */
  virtual void Set(double speed) = 0;

  /**
   * Sets the voltage output of the SpeedController.  Compensates for
   * the current bus voltage to ensure that the desired voltage is output even
   * if the battery voltage is below 12V - highly useful when the voltage
   * outputs are "meaningful" (e.g. they come from a feedforward calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage
   * compensation to work properly - unlike the ordinary set function, it is not
   * "set it and forget it."
   *
   * @param output The voltage to output.
   */
  virtual void SetVoltage(units::volt_t output);

  /**
   * Common interface for getting the current set speed of a speed controller.
   *
   * @return The current set speed.  Value is between -1.0 and 1.0.
   */
  virtual double Get() const = 0;

  /**
   * Common interface for inverting direction of a speed controller.
   *
   * @param isInverted The state of inversion, true is inverted.
   */
  virtual void SetInverted(bool isInverted) = 0;

  /**
   * Common interface for returning the inversion state of a speed controller.
   *
   * @return isInverted The state of inversion, true is inverted.
   */
  virtual bool GetInverted() const = 0;

  /**
   * Common interface for disabling a motor.
   */
  virtual void Disable() = 0;

  /**
   * Common interface to stop the motor until Set is called again.
   */
  virtual void StopMotor() = 0;
};

}  // namespace frc
