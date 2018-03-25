/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ClosedLoopMotor.h"

using namespace frc;

/**
 * Allocate a controlled motor with the given constants for Kp, Ki, and Kd.
 *
 * @param Kp     The proportional coefficient.
 * @param Ki     The integral coefficient.
 * @param Kd     The derivative coefficient.
 * @param input  The INode object that is used to get values.
 * @param motor  The SpeedController object that is set to the output value.
 * @param period The loop time for doing calculations.
 */
ClosedLoopMotor::ClosedLoopMotor(double Kp, double Ki, double Kd, INode& input,
                                 SpeedController& motor, double period)
    : m_sum(m_refInput, true, input, false),
      m_pid(Kp, Ki, Kd, m_sum, period),
      m_output(m_pid, motor, period) {}

/**
 * Allocate a controlled motor with the given constants for Kp, Ki, Kd, and Kff.
 *
 * @param Kp     The proportional coefficient.
 * @param Ki     The integral coefficient.
 * @param Kd     The derivative coefficient.
 * @param Kff    The feedforward coefficient.
 * @param input  The INode object that is used to get values.
 * @param motor  The SpeedController object that is set to the output value.
 * @param period The loop time for doing calculations.
 */
ClosedLoopMotor::ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff,
                                 INode& input, SpeedController& motor,
                                 double period)
    : m_feedforward(Kff, m_refInput),
      m_sum(m_refInput, true, input, false),
      m_pid(Kp, Ki, Kd, m_feedforward, m_sum, period),
      m_output(m_pid, motor, period) {}

/**
 * When Set() is called, its [-1..1] range is mapped to the given displacement
 * range before being set as the controller's reference.
 *
 * @param minimumDisplacement Set(-1.0) maps to this value.
 * @param maximumDisplacement Set(1.0) maps to this value.
 * @param continuous          Whether the position control should consider the
 *                            input continuous.
 */
void ClosedLoopMotor::SetDisplacementRange(double minimumDisplacement,
                                           double maximumDisplacement,
                                           bool continuous) {
  m_minimumInput = minimumDisplacement;
  m_maximumInput = maximumDisplacement;
  m_sum.SetInputRange(m_minimumInput, m_maximumInput);
  m_sum.SetContinuous(continuous);
}

/**
 * When Set() is called, its [-1..1] range is mapped to the given velocity range
 * before being set as the controller's reference.
 *
 * @param minimumVelocity Set(-1.0) maps to this value.
 * @param maximumVelocity Set(1.0) maps to this value.
 */
void ClosedLoopMotor::SetVelocityRange(double minimumVelocity,
                                       double maximumVelocity) {
  m_minimumInput = minimumVelocity;
  m_maximumInput = maximumVelocity;
  m_sum.SetInputRange(m_minimumInput, m_maximumInput);
}

/**
 * Enables the internal controller.
 */
void ClosedLoopMotor::Enable() { m_output.Enable(); }

/**
 * Returns the internal controller.
 */
PIDNode& ClosedLoopMotor::GetController() { return m_pid; }

void ClosedLoopMotor::Set(double speed) {
  if (m_isInverted) {
    speed = -speed;
  }

  m_refInput.Set(m_minimumInput +
                 (m_maximumInput - m_minimumInput) * (speed + 1.0) / 2.0);
}

/**
 * Converts the internal controller's reference to a SpeedController value in
 * the range [-1..1] using the controller input range, then returns it.
 */
double ClosedLoopMotor::Get() const {
  return 2.0 * (m_refInput.GetOutput() - m_minimumInput) /
             (m_maximumInput - m_minimumInput) -
         1.0;
}

void ClosedLoopMotor::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool ClosedLoopMotor::GetInverted() const { return m_isInverted; }

void ClosedLoopMotor::Disable() { StopMotor(); }

void ClosedLoopMotor::StopMotor() { m_output.Disable(); }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController.
 */
void ClosedLoopMotor::PIDWrite(double output) { Set(output); }

void ClosedLoopMotor::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("ClosedLoopMotor");
  builder.SetSafeState([=] {
    m_output.Disable();
    m_pid.Reset();
  });
}
