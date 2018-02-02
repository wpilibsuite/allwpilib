/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PIDController.h"
#include "frc/PIDSource.h"
#include "frc/SpeedController.h"
#include "frc/smartdashboard/SendableBase.h"
#include "frc/smartdashboard/SendableBuilder.h"

namespace frc {

/**
 * A class that performs closed-loop velocity control on a motor.
 *
 * It implements SpeedController so this control can be used transparently
 * within drive bases.
 *
 * Call either SetDisplacementRange() or SetVelocityRange(), then Enable()
 * before use.
 */
class ClosedLoopMotor : public SpeedController, public SendableBase {
 public:
  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  ClosedLoopMotor(double Kp, double Ki, double Kd, PIDSource& source,
                  SpeedController& motor, double period = 0.05);

  /**
   * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and
   * Kff.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param Kff    The feedforward coefficient.
   * @param source The PIDSource object that is used to get values.
   * @param motor  The SpeedController object that is set to the output value.
   * @param period The loop time for doing calculations.
   */
  ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff,
                  PIDSource& source, SpeedController& motor,
                  double period = 0.05);

  /**
   * When Set() is called, its [-1..1] range is mapped to the given displacement
   * range before being set as the controller's reference.
   *
   * This enables position control internally.
   *
   * @param minimumDisplacement Set(-1.0) maps to this value.
   * @param maximumDisplacement Set(1.0) maps to this value.
   * @param continuous          Whether the position control should consider the
   *                            input continuous.
   */
  void SetDisplacementRange(double minimumDisplacement,
                            double maximumDisplacement,
                            bool continuous = false);

  /**
   * When Set() is called, its [-1..1] range is mapped to the given velocity
   * range before being set as the controller's reference.
   *
   * This enables velocity control internally.
   *
   * @param minimumVelocity Set(-1.0) maps to this value.
   * @param maximumVelocity Set(1.0) maps to this value.
   */
  void SetVelocityRange(double minimumVelocity, double maximumVelocity);

  /**
   * Enables the internal controller.
   */
  void Enable();

  /**
   * Returns the internal controller.
   */
  PIDController& GetController();

  void Set(double speed) override;

  /**
   * Converts the internal controller's reference to a SpeedController value in
   * the range [-1..1] using the controller's input range, then returns it.
   */
  double Get() const override;

  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  /**
   * Passes the output directly to SetSetpoint().
   *
   * PIDControllers can be nested by passing a PIDController as another
   * PIDController's output. In that case, the output of the parent controller
   * becomes the input (i.e., the reference) of the child.
   *
   * It is the caller's responsibility to put the data into a valid form for
   * SetSetpoint().
   */
  void PIDWrite(double output) override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  PIDController m_controller;
  double m_minimumInput = -1.0;
  double m_maximumInput = 1.0;
  double m_inputRange = m_maximumInput - m_minimumInput;
  bool m_isInverted = false;
};

}  // namespace frc
