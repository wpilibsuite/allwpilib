/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/commands/PIDSubsystem.h"

using namespace frc::experimental;

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double Kp, double Ki,
                           double Kd)
    : Subsystem(name),
      m_controller(Kp, Ki, Kd, *this),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double Kp, double Ki,
                           double Kd, std::function<double()> feedforward)
    : Subsystem(name),
      m_controller(Kp, Ki, Kd, feedforward, *this),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double Kp, double Ki,
                           double Kd, std::function<double()> feedforward,
                           double period)
    : Subsystem(name),
      m_controller(Kp, Ki, Kd, feedforward, *this, period),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double Kp, double Ki, double Kd)
    : Subsystem("PIDSubsystem"),
      m_controller(Kp, Ki, Kd, *this),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double Kp, double Ki, double Kd,
                           std::function<double()> feedforward)
    : Subsystem("PIDSubsystem"),
      m_controller(Kp, Ki, Kd, feedforward),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double Kp, double Ki, double Kd,
                           std::function<double()> feedforward, double period)
    : Subsystem("PIDSubsystem"),
      m_controller(Kp, Ki, Kd, feedforward, period),
      m_runner(m_controller, *this) {
  AddChild("PIDController", m_controller);
}

void PIDSubsystem::Enable() { m_runner.Enable(); }

void PIDSubsystem::Disable() { m_runner.Disable(); }

void PIDSubsystem::SetReference(double reference) {
  m_controller.SetReference(reference);
}

void PIDSubsystem::SetReferenceRelative(double deltaReference) {
  SetReference(GetReference() + deltaReference);
}

void PIDSubsystem::SetInputRange(double minimumInput, double maximumInput) {
  m_controller.SetInputRange(minimumInput, maximumInput);
}

void PIDSubsystem::SetOutputRange(double minimumOutput, double maximumOutput) {
  m_controller.SetOutputRange(minimumOutput, maximumOutput);
}

double PIDSubsystem::GetReference() const {
  return m_controller.GetReference();
}

void PIDSubsystem::SetAbsoluteTolerance(double tolerance,
                                        double deltaTolerance) {
  m_controller.SetAbsoluteTolerance(tolerance, deltaTolerance);
}

void PIDSubsystem::SetPercentTolerance(double tolerance,
                                       double deltaTolerance) {
  m_controller.SetPercentTolerance(tolerance, deltaTolerance);
}

bool PIDSubsystem::AtReference() const { return m_controller.AtReference(); }

PIDController& PIDSubsystem::GetPIDController() { return m_controller; }
