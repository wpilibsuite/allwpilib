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
   * Sets the output power level of the motor controller, expressed as a
   * fraction of the maximum possible.
   *
   * This is generally perceived as "speed" of the output for a fixed load, but
   * the actual speed will depend on the applied load.  For example, a value
   * of 1.0 will result in the motor outputting full power in the forward
   * direction, so it will move as fast as it can for the amount of load it is
   * under, which will be slower for heavier loads and faster for lighter loads.
   *
   * The maximum possible output is determined by the battery voltage, so a
   * value of e.g. 0.5 will result in a different amount of motor power being
   * applied for a fully charged battery vs. a nearly depleted battery. For
   * consistent output that is independent of battery voltage, use SetVoltage(),
   * potentially combined with closed-loop control that uses a sensor to control
   * the amount of applied output.
   *
   * @param power The power level, where -1.0 indicates full reverse and 1.0
   *     indicates full forward.
   */
  virtual void SetPower(double power) = 0;

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
   * Gets the power level of the motor controller.
   *
   * @return The power level, where -1.0 represents full reverse and 1.0
   *     represents full forward.
   */
  virtual double GetPower() const = 0;

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
