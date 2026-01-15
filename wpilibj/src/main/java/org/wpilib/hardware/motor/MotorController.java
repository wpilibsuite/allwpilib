// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

import static org.wpilib.units.Units.Volts;

import org.wpilib.system.RobotController;
import org.wpilib.units.measure.Voltage;

/** Interface for motor controlling devices. */
public interface MotorController {
  /**
   * Sets the duty cycle of the motor controller.
   *
   * @param dutyCycle The duty cycle between -1 and 1 (sign indicates direction).
   */
  void setDutyCycle(double dutyCycle);

  /**
   * Sets the voltage output of the motor controller.
   *
   * <p>Compensates for the current bus voltage to ensure that the desired voltage is output even if
   * the battery voltage is below 12V - highly useful when the voltage outputs are "meaningful"
   * (e.g. they come from a feedforward calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage compensation to work
   * properly - unlike the ordinary set function, it is not "set it and forget it."
   *
   * @param voltage The voltage.
   */
  default void setVoltage(double voltage) {
    setDutyCycle(voltage / RobotController.getBatteryVoltage());
  }

  /**
   * Sets the voltage output of the motor controller.
   *
   * <p>Compensates for the current bus voltage to ensure that the desired voltage is output even if
   * the battery voltage is below 12V - highly useful when the voltage outputs are "meaningful"
   * (e.g. they come from a feedforward calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage compensation to work
   * properly - unlike the ordinary set function, it is not "set it and forget it."
   *
   * @param voltage The voltage.
   */
  default void setVoltage(Voltage voltage) {
    setVoltage(voltage.in(Volts));
  }

  /**
   * Gets the duty cycle of the motor controller.
   *
   * @return The duty cycle between -1 and 1 (sign indicates direction).
   */
  double getDutyCycle();

  /**
   * Sets the inversion state of the motor controller.
   *
   * @param isInverted The inversion state.
   */
  void setInverted(boolean isInverted);

  /**
   * Gets the inversion state of the motor controller.
   *
   * @return The inversion state.
   */
  boolean getInverted();

  /** Disables the motor controller. */
  void disable();
}
