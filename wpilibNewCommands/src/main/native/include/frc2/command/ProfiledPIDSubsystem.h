/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/controller/ProfiledPIDController.h>
#include <units/units.h>

#include "frc2/command/SubsystemBase.h"

namespace frc2 {
/**
 * A subsystem that uses a ProfiledPIDController to control an output.  The
 * controller is run synchronously from the subsystem's periodic() method.
 *
 * @see ProfiledPIDController
 */
class ProfiledPIDSubsystem : public SubsystemBase {
  using State = frc::TrapezoidProfile::State;

 public:
  /**
   * Creates a new ProfiledPIDSubsystem.
   *
   * @param controller the ProfiledPIDController to use
   */
  explicit ProfiledPIDSubsystem(frc::ProfiledPIDController controller);

  void Periodic() override;

  /**
   * Uses the output from the ProfiledPIDController.
   *
   * @param output the output of the ProfiledPIDController
   * @param setpoint the setpoint state of the ProfiledPIDController, for
   * feedforward
   */
  virtual void UseOutput(double output, State setpoint) = 0;

  /**
   * Returns the goal used by the ProfiledPIDController.
   *
   * @return the goal to be used by the controller
   */
  virtual State GetGoal() = 0;

  /**
   * Returns the measurement of the process variable used by the
   * ProfiledPIDController.
   *
   * @return the measurement of the process variable
   */
  virtual units::meter_t GetMeasurement() = 0;

  /**
   * Enables the PID control.  Resets the controller.
   */
  virtual void Enable();

  /**
   * Disables the PID control.  Sets output to zero.
   */
  virtual void Disable();

  /**
   * Returns the ProfiledPIDController.
   *
   * @return The controller.
   */
  frc::ProfiledPIDController& GetController();

 protected:
  frc::ProfiledPIDController m_controller;
  bool m_enabled;
};
}  // namespace frc2
