/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/PIDSubsystem.h"

#include "frc/PIDController.h"

using namespace frc;

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double p, double i, double d)
    : Subsystem(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double p, double i, double d,
                           double f)
    : Subsystem(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, f, this, this);
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(const wpi::Twine& name, double p, double i, double d,
                           double f, double period)
    : Subsystem(name) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double p, double i, double d)
    : Subsystem("PIDSubsystem") {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double p, double i, double d, double f)
    : Subsystem("PIDSubsystem") {
  m_controller = std::make_shared<PIDController>(p, i, d, f, this, this);
  AddChild("PIDController", m_controller);
}

PIDSubsystem::PIDSubsystem(double p, double i, double d, double f,
                           double period)
    : Subsystem("PIDSubsystem") {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
  AddChild("PIDController", m_controller);
}

void PIDSubsystem::Enable() { m_controller->Enable(); }

void PIDSubsystem::Disable() { m_controller->Disable(); }

void PIDSubsystem::PIDWrite(double output) { UsePIDOutput(output); }

double PIDSubsystem::PIDGet() { return ReturnPIDInput(); }

void PIDSubsystem::SetSetpoint(double setpoint) {
  m_controller->SetSetpoint(setpoint);
}

void PIDSubsystem::SetSetpointRelative(double deltaSetpoint) {
  SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDSubsystem::SetInputRange(double minimumInput, double maximumInput) {
  m_controller->SetInputRange(minimumInput, maximumInput);
}

void PIDSubsystem::SetOutputRange(double minimumOutput, double maximumOutput) {
  m_controller->SetOutputRange(minimumOutput, maximumOutput);
}

double PIDSubsystem::GetSetpoint() { return m_controller->GetSetpoint(); }

double PIDSubsystem::GetPosition() { return ReturnPIDInput(); }

double PIDSubsystem::GetRate() { return ReturnPIDInput(); }

void PIDSubsystem::SetAbsoluteTolerance(double absValue) {
  m_controller->SetAbsoluteTolerance(absValue);
}

void PIDSubsystem::SetPercentTolerance(double percent) {
  m_controller->SetPercentTolerance(percent);
}

bool PIDSubsystem::OnTarget() const { return m_controller->OnTarget(); }

std::shared_ptr<PIDController> PIDSubsystem::GetPIDController() {
  return m_controller;
}
