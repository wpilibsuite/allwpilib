// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Interface for motor controlling devices. */
public interface MotorController {
  /**
   * Common interface for setting the speed of a motor controller.
   *
   * @param speed The speed to set. Value should be between -1.0 and 1.0.
   */
  void set(double speed);

  /**
   * Sets the voltage output of the MotorController. Compensates for the current bus voltage to
   * ensure that the desired voltage is output even if the battery voltage is below 12V - highly
   * useful when the voltage outputs are "meaningful" (e.g. they come from a feedforward
   * calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage compensation to work
   * properly - unlike the ordinary set function, it is not "set it and forget it."
   *
   * @param outputVolts The voltage to output, in Volts.
   */
  default void setVoltage(double outputVolts) {
    set(outputVolts / RobotController.getBatteryVoltage());
  }

  /**
   * Sets the voltage output of the MotorController. Compensates for the current bus voltage to
   * ensure that the desired voltage is output even if the battery voltage is below 12V - highly
   * useful when the voltage outputs are "meaningful" (e.g. they come from a feedforward
   * calculation).
   *
   * <p>NOTE: This function *must* be called regularly in order for voltage compensation to work
   * properly - unlike the ordinary set function, it is not "set it and forget it."
   *
   * @param outputVoltage The voltage to output.
   */
  default void setVoltage(Voltage outputVoltage) {
    setVoltage(outputVoltage.in(Volts));
  }

  /**
   * Common interface for getting the current set speed of a motor controller.
   *
   * @return The current set speed. Value is between -1.0 and 1.0.
   */
  double get();

  /**
   * Common interface for inverting direction of a motor controller.
   *
   * @param isInverted The state of inversion true is inverted.
   */
  void setInverted(boolean isInverted);

  /**
   * Common interface for returning if a motor controller is in the inverted state or not.
   *
   * @return isInverted The state of the inversion true is inverted.
   */
  boolean getInverted();

  /** Disable the motor controller. */
  void disable();

  /**
   * Stops motor movement. Motor can be moved again by calling set without having to re-enable the
   * motor.
   */
  void stopMotor();
}
