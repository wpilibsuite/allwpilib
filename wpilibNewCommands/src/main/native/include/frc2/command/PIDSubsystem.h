/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
   */
  explicit PIDSubsystem(PIDController controller);

  void Periodic() override;

  /**
   * Uses the output from the PIDController.
   *
   * @param output the output of the PIDController
   */
  virtual void UseOutput(double output) = 0;

  /**
   * Returns the reference (setpoint) used by the PIDController.
   *
   * @return the reference (setpoint) to be used by the controller
   */
  virtual double GetSetpoint() = 0;

  /**
   * Returns the measurement of the process variable used by the PIDController.
   *
   * @return the measurement of the process variable
   */
  virtual double GetMeasurement() = 0;

  /**
   * Enables the PID control.  Resets the controller.
   */
  virtual void Enable();

  /**
   * Disables the PID control.  Sets output to zero.
   */
  virtual void Disable();

  /**
   * Returns the PIDController.
   *
   * @return The controller.
   */
  PIDController& GetController();

 protected:
  PIDController m_controller;
  bool m_enabled;
};
}  // namespace frc2
