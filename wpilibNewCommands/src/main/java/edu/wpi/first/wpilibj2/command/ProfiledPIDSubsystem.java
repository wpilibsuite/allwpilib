/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.controller.ProfiledPIDController;

import static edu.wpi.first.wpilibj.trajectory.TrapezoidProfile.State;
import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A subsystem that uses a {@link ProfiledPIDController} to control an output.  The controller is
 * run synchronously from the subsystem's periodic() method.
 */
public abstract class ProfiledPIDSubsystem extends SubsystemBase {
  protected final ProfiledPIDController m_controller;
  protected boolean m_enabled;

  /**
   * Creates a new ProfiledPIDSubsystem.
   *
   * @param controller the ProfiledPIDController to use
   */
  public ProfiledPIDSubsystem(ProfiledPIDController controller) {
    requireNonNullParam(controller, "controller", "ProfiledPIDSubsystem");
    m_controller = controller;
  }

  @Override
  public void periodic() {
    if (m_enabled) {
      useOutput(m_controller.calculate(getMeasurement(), getGoal()), m_controller.getSetpoint());
    }
  }

  public ProfiledPIDController getController() {
    return m_controller;
  }

  /**
   * Uses the output from the ProfiledPIDController.
   *
   * @param output   the output of the ProfiledPIDController
   * @param setpoint the setpoint state of the ProfiledPIDController, for feedforward
   */
  public abstract void useOutput(double output, State setpoint);

  /**
   * Returns the goal used by the ProfiledPIDController.
   *
   * @return the goal to be used by the controller
   */
  public abstract State getGoal();

  /**
   * Returns the measurement of the process variable used by the ProfiledPIDController.
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
    useOutput(0, new State());
  }
}
