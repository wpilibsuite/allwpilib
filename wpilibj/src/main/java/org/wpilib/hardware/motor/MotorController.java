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
   * Sets the output power level of the motor controller, expressed as a fraction of the maximum
   * possible.
   *
   * <p>This is generally perceived as "speed" of the output for a fixed load, but the actual speed
   * will depend on the applied load. For example, a value of 1.0 will result in the motor
   * outputting full power in the forward direction, so it will move as fast as it can for the
   * amount of load it is under, which will be slower for heavier loads and faster for lighter
   * loads.
   *
   * <p>The maximum possible output is determined by the battery voltage, so a value of e.g. 0.5
   * will result in a different amount of motor power being applied for a fully charged battery vs.
   * a nearly depleted battery. For consistent output that is independent of battery voltage, use
   * {@link #setVoltage(double)}, potentially combined with closed-loop control that uses a sensor
   * to control the amount of applied output.
   *
   * @param power The power level, where -1.0 indicates full reverse and 1.0 indicates full forward.
   */
  void setPower(double power);

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
    setPower(voltage / RobotController.getBatteryVoltage());
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
   * Gets the power level of the motor controller.
   *
   * @return The power level, where -1.0 represents full reverse and 1.0 represents full forward.
   */
  double getPower();

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
