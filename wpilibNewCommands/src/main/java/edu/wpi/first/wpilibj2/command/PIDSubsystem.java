/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.controller.PIDController;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A subsystem that uses a {@link PIDController} to control an output.  The controller is run
 * synchronously from the subsystem's periodic() method.
 */
public abstract class PIDSubsystem extends SubsystemBase {
  protected final PIDController m_controller;
  protected boolean m_enabled;

  /**
   * Creates a new PIDSubsystem.
   *
   * @param controller the PIDController to use
   */
  public PIDSubsystem(PIDController controller) {
    requireNonNullParam(controller, "controller", "PIDSubsystem");
    m_controller = controller;
  }

  @Override
  public void periodic() {
    if (m_enabled) {
      useOutput(m_controller.calculate(getMeasurement(), getSetpoint()));
    }
  }

  public PIDController getController() {
    return m_controller;
  }

  /**
   * Uses the output from the PIDController.
   *
   * @param output the output of the PIDController
   */
  public abstract void useOutput(double output);

  /**
   * Returns the reference (setpoint) used by the PIDController.
   *
   * @return the reference (setpoint) to be used by the controller
   */
  public abstract double getSetpoint();

  /**
   * Returns the measurement of the process variable used by the PIDController.
   *
   * @return the measurement of the process variable
   */
  public abstract double getMeasurement();

  /**
   * Enables the PID control.  Resets the controller.
   */
  public void enable() {
    m_enabled = true;
    m_controller.reset();
  }

  /**
   * Disables the PID control.  Sets output to zero.
   */
  public void disable() {
    m_enabled = false;
    useOutput(0);
  }
}
