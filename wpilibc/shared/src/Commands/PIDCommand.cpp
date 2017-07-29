/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/PIDCommand.h"

#include <cfloat>

frc::PIDCommand::PIDCommand(const std::string& name, double p, double i,
                            double d, double f, double period)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

frc::PIDCommand::PIDCommand(double p, double i, double d, double f,
                            double period) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

frc::PIDCommand::PIDCommand(const std::string& name, double p, double i,
                            double d)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

frc::PIDCommand::PIDCommand(const std::string& name, double p, double i,
                            double d, double period)
    : Command(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

frc::PIDCommand::PIDCommand(double p, double i, double d) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

frc::PIDCommand::PIDCommand(double p, double i, double d, double period) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this, period);
}

void frc::PIDCommand::_Initialize() { m_controller->Enable(); }

void frc::PIDCommand::_End() { m_controller->Disable(); }

void frc::PIDCommand::_Interrupted() { _End(); }

void frc::PIDCommand::SetSetpointRelative(double deltaSetpoint) {
  SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void frc::PIDCommand::PIDWrite(double output) { UsePIDOutput(output); }

double frc::PIDCommand::PIDGet() { return ReturnPIDInput(); }

std::shared_ptr<frc::PIDController> frc::PIDCommand::GetPIDController() const {
  return m_controller;
}

void frc::PIDCommand::SetSetpoint(double setpoint) {
  m_controller->SetSetpoint(setpoint);
}

double frc::PIDCommand::GetSetpoint() const {
  return m_controller->GetSetpoint();
}

double frc::PIDCommand::GetPosition() { return ReturnPIDInput(); }

std::string frc::PIDCommand::GetSmartDashboardType() const {
  return "PIDCommand";
}

void frc::PIDCommand::InitTable(std::shared_ptr<ITable> subtable) {
  m_controller->InitTable(subtable);
  Command::InitTable(subtable);
}
