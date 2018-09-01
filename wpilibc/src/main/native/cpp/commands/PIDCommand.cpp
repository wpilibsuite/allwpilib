/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/PIDCommand.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d,
                       double f, double period)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d, double f, double period) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d,
                       double period)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(double p, double i, double d, double period) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d,
                       double f, double period, Subsystem& subsystem)
    : Command(name, subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d, double f, double period,
                       Subsystem& subsystem)
    : Command(subsystem) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d,
                       Subsystem& subsystem)
    : Command(name, subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(const wpi::Twine& name, double p, double i, double d,
                       double period, Subsystem& subsystem)
    : Command(name, subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d, Subsystem& subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(double p, double i, double d, double period,
                       Subsystem& subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

void PIDCommand::_Initialize() { m_controller->Enable(); }

void PIDCommand::_End() { m_controller->Disable(); }

void PIDCommand::_Interrupted() { _End(); }

void PIDCommand::SetSetpointRelative(double deltaSetpoint) {
  SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDCommand::PIDWrite(double output) { UsePIDOutput(output); }

double PIDCommand::PIDGet() { return ReturnPIDInput(); }

std::shared_ptr<PIDController> PIDCommand::GetPIDController() const {
  return m_controller;
}

void PIDCommand::SetSetpoint(double setpoint) {
  m_controller->SetSetpoint(setpoint);
}

double PIDCommand::GetSetpoint() const { return m_controller->GetSetpoint(); }

double PIDCommand::GetPosition() { return ReturnPIDInput(); }

void PIDCommand::InitSendable(SendableBuilder& builder) {
  m_controller->InitSendable(builder);
  Command::InitSendable(builder);
  builder.SetSmartDashboardType("PIDCommand");
}
