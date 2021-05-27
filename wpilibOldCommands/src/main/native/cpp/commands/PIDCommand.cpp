// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/PIDCommand.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d,
                       double f, double period)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d, double f, double period) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d,
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

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d,
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

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d,
                       Subsystem& subsystem)
    : Command(name, subsystem) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

PIDCommand::PIDCommand(std::string_view name, double p, double i, double d,
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

void PIDCommand::_Initialize() {
  m_controller->Enable();
}

void PIDCommand::_End() {
  m_controller->Disable();
}

void PIDCommand::_Interrupted() {
  _End();
}

void PIDCommand::SetSetpointRelative(double deltaSetpoint) {
  SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDCommand::PIDWrite(double output) {
  UsePIDOutput(output);
}

double PIDCommand::PIDGet() {
  return ReturnPIDInput();
}

std::shared_ptr<PIDController> PIDCommand::GetPIDController() const {
  return m_controller;
}

void PIDCommand::SetSetpoint(double setpoint) {
  m_controller->SetSetpoint(setpoint);
}

double PIDCommand::GetSetpoint() const {
  return m_controller->GetSetpoint();
}

double PIDCommand::GetPosition() {
  return ReturnPIDInput();
}

void PIDCommand::InitSendable(SendableBuilder& builder) {
  m_controller->InitSendable(builder);
  Command::InitSendable(builder);
  builder.SetSmartDashboardType("PIDCommand");
}
