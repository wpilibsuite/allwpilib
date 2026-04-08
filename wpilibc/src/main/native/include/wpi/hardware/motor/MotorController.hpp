// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/voltage.hpp"

namespace wpi {

/**
 * Interface for motor controlling devices.
 */
class MotorController {
 public:
  virtual ~MotorController() = default;

  /**
   * Sets the duty cycle of the motor controller.
   *
   * @param dutyCycle The duty cycle between -1 and 1 (sign indicates
   *     direction).
   */
  virtual void SetDutyCycle(double dutyCycle) = 0;

  /**
   * Sets the voltage output of the motor controller.
   *
   * Compensates for the current bus voltage to ensure that the desired voltage
   * is output even if the battery voltage is below 12V - highly useful when the
   * voltage outputs are "meaningful" (e.g. they come from a feedforward
   * calculation).
   *
   * NOTE: This function *must* be called regularly in order for voltage
   * compensation to work properly - unlike the ordinary set function, it is not
   * "set it and forget it."
   *
   * @param voltage The voltage.
   */
  virtual void SetVoltage(wpi::units::volt_t voltage);

  /**
   * Gets the duty cycle of the motor controller.
   *
   * @return The duty cycle between -1 and 1 (sign indicates direction).
   */
  virtual double GetDutyCycle() const = 0;

  /**
   * Sets the inversion state of the motor controller.
   *
   * @param isInverted The inversion state.
   */
  virtual void SetInverted(bool isInverted) = 0;

  /**
   * Gets the inversion state of the motor controller.
   *
   * @return The inversion state.
   */
  virtual bool GetInverted() const = 0;

  /**
   * Disables the motor controller.
   */
  virtual void Disable() = 0;
};

}  // namespace wpi
