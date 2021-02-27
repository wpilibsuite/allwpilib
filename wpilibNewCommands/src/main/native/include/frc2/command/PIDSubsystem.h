// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/controller/PIDController.h>

#include "frc2/command/SubsystemBase.h"

namespace frc2 {
/**
 * A subsystem that uses a PIDController to control an output.  The controller
 * is run synchronously from the subsystem's periodic() method.
 *
 * @see PIDController
 */
class PIDSubsystem : public SubsystemBase {
 public:
  /**
   * Creates a new PIDSubsystem.
   *
   * @param controller the PIDController to use
   * @param initialPosition the initial setpoint of the subsystem
   */
  explicit PIDSubsystem(PIDController controller, double initialPosition = 0);

  void Periodic() override;

  /**
   * Sets the setpoint for the subsystem.
   *
   * @param setpoint the setpoint for the subsystem
   */
  void SetSetpoint(double setpoint);

  /**
   * Gets the setpoint for the subsystem.
   *
   * @return the setpoint for the subsystem
   */
  double GetSetpoint() const;

  /**
   * Enables the PID control.  Resets the controller.
   */
  virtual void Enable();

  /**
   * Disables the PID control.  Sets output to zero.
   */
  virtual void Disable();

  /**
   * Returns whether the controller is enabled.
   *
   * @return Whether the controller is enabled.
   */
  bool IsEnabled();

  /**
   * Returns the PIDController.
   *
   * @return The controller.
   */
  PIDController& GetController();

 protected:
  PIDController m_controller;
  bool m_enabled{false};

  /**
   * Returns the measurement of the process variable used by the PIDController.
   *
   * @return the measurement of the process variable
   */
  virtual double GetMeasurement() = 0;

  /**
   * Uses the output from the PIDController.
   *
   * @param output the output of the PIDController
   * @param setpoint the setpoint of the PIDController (for feedforward)
   */
  virtual void UseOutput(double output, double setpoint) = 0;

 private:
  double m_setpoint{0};
};
}  // namespace frc2
