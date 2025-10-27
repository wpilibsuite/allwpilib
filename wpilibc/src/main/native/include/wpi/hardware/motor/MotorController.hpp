// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/voltage.h>

namespace frc {

/**
 * Interface for motor controlling devices.
 */
class MotorController {
 public:
  virtual ~MotorController() = default;

  /**
   * Common interface for setting the speed of a motor controller.
   *
   * @param speed The speed to set.  Value should be between -1.0 and 1.0.
   */
  virtual void Set(double speed) = 0;

  /**
   * Sets the voltage output of the MotorController.  Compensates for
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
   * Common interface for getting the current set speed of a motor controller.
   *
   * @return The current set speed.  Value is between -1.0 and 1.0.
   */
  virtual double Get() const = 0;

  /**
   * Common interface for inverting direction of a motor controller.
   *
   * @param isInverted The state of inversion, true is inverted.
   */
  virtual void SetInverted(bool isInverted) = 0;

  /**
   * Common interface for returning the inversion state of a motor controller.
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
