/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/ClosedLoopMotor.h"

using namespace frc;

ClosedLoopMotor::ClosedLoopMotor(double Kp, double Ki, double Kd,
                                 PIDSource& source, SpeedController& motor,
                                 double period)
    : ClosedLoopMotor(Kp, Ki, Kd, 0.0, source, motor, period) {}

ClosedLoopMotor::ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff,
                                 PIDSource& source, SpeedController& motor,
                                 double period)
    : m_controller(Kp, Ki, Kd, Kff, source, motor, period) {
  AddChild(&m_controller);
}

void ClosedLoopMotor::SetDisplacementRange(double minimumDisplacement,
                                           double maximumDisplacement,
                                           bool continuous) {
  m_minimumInput = minimumDisplacement;
  m_maximumInput = maximumDisplacement;
  m_inputRange = m_maximumInput - m_minimumInput;
  m_controller.SetPIDSourceType(PIDSourceType::kDisplacement);
  m_controller.SetInputRange(m_minimumInput, m_maximumInput);
  m_controller.SetContinuous(continuous);
}

void ClosedLoopMotor::SetVelocityRange(double minimumVelocity,
                                       double maximumVelocity) {
  m_minimumInput = minimumVelocity;
  m_maximumInput = maximumVelocity;
  m_controller.SetPIDSourceType(PIDSourceType::kRate);
  m_controller.SetInputRange(m_minimumInput, m_maximumInput);
}

void ClosedLoopMotor::Enable() { m_controller.Enable(); }

PIDController& ClosedLoopMotor::GetController() { return m_controller; }

void ClosedLoopMotor::Set(double speed) {
  if (m_isInverted) {
    speed = -speed;
  }

  m_controller.SetSetpoint(m_minimumInput + m_inputRange * (speed + 1.0) / 2.0);
}

double ClosedLoopMotor::Get() const {
  return 2.0 * (m_controller.GetSetpoint() - m_minimumInput) / m_inputRange -
         1.0;
}

void ClosedLoopMotor::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool ClosedLoopMotor::GetInverted() const { return m_isInverted; }

void ClosedLoopMotor::Disable() { StopMotor(); }

void ClosedLoopMotor::StopMotor() { m_controller.Disable(); }

void ClosedLoopMotor::PIDWrite(double output) {
  m_controller.SetSetpoint(output);
}

void ClosedLoopMotor::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("ClosedLoopMotor");
  builder.SetSafeState([=] { m_controller.Reset(); });
}
