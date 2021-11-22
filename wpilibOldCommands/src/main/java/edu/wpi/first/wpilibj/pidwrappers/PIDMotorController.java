// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.pidwrappers;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;

/**
 * Wrapper so that PIDOutput is implemented for MotorController for old PIDController.
 */
public class PIDMotorController implements PIDOutput, MotorController, Sendable {
  private final MotorController m_motorController;

  public PIDMotorController(MotorController motorController) {
    m_motorController = motorController;
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  @Override
  public void pidWrite(double output) {
    m_motorController.set(output);
  }

  /**
   * Common interface for setting the speed of a motor controller.
   *
   * @param speed The speed to set. Value should be between -1.0 and 1.0.
   */
  @Override
  public void set(double speed) {
    m_motorController.set(speed);
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
   * @param outputVolts The voltage to output.
   */
  @Override
  public void setVoltage(double outputVolts) {
    m_motorController.setVoltage(outputVolts);
  }

  /**
   * Common interface for getting the current set speed of a motor controller.
   *
   * @return The current set speed. Value is between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return m_motorController.get();
  }

  /**
   * Common interface for inverting direction of a motor controller.
   *
   * @param isInverted The state of inversion true is inverted.
   */
  @Override
  public void setInverted(boolean isInverted) {
    m_motorController.setInverted(isInverted);
  }

  /**
   * Common interface for returning if a motor controller is in the inverted state or not.
   *
   * @return isInverted The state of the inversion true is inverted.
   */
  @Override
  public boolean getInverted() {
    return m_motorController.getInverted();
  }

  /** Disable the motor controller. */
  @Override
  public void disable() {
    m_motorController.disable();
  }

  /**
   * Stops motor movement. Motor can be moved again by calling set without having to re-enable the
   * motor.
   */
  @Override
  public void stopMotor() {
    m_motorController.stopMotor();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Motor Controller");
    builder.setActuator(true);
    builder.setSafeState(this::disable);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}
