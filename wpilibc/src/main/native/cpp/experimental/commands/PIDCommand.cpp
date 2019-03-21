/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/commands/PIDCommand.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc::experimental;

PIDCommand::PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd)
    : Command(name),
      m_controller(Kp, Ki, Kd, *this),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
                       double period)
    : Command(name),
      m_controller(Kp, Ki, Kd, *this, period),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd,
                       std::function<double()> feedforward, double period)
    : m_controller(Kp, Ki, Kd, feedforward, *this, period),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd)
    : m_controller(Kp, Ki, Kd, *this), m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd, double period)
    : m_controller(Kp, Ki, Kd, *this, period), m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
                       Subsystem& subsystem)
    : Command(name, subsystem),
      m_controller(Kp, Ki, Kd, *this),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
                       double period, Subsystem& subsystem)
    : Command(name, subsystem),
      m_controller(Kp, Ki, Kd, *this, period),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
                       std::function<double()> feedforward, double period,
                       Subsystem& subsystem)
    : Command(name, subsystem),
      m_controller(Kp, Ki, Kd, feedforward, *this, period),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd, Subsystem& subsystem)
    : Command(subsystem),
      m_controller(Kp, Ki, Kd, *this),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd, double period,
                       Subsystem& subsystem)
    : Command(subsystem),
      m_controller(Kp, Ki, Kd, *this, period),
      m_runner(m_controller, *this) {}

PIDCommand::PIDCommand(double Kp, double Ki, double Kd,
                       std::function<double()> feedforward, double period,
                       Subsystem& subsystem)
    : Command(subsystem),
      m_controller(Kp, Ki, Kd, feedforward, *this, period),
      m_runner(m_controller, *this) {}

void PIDCommand::InitSendable(SendableBuilder& builder) {
  m_controller.InitSendable(builder);
  Command::InitSendable(builder);
  builder.SetSmartDashboardType("PIDCommand");
}

void PIDCommand::_Initialize() { m_runner.Enable(); }

void PIDCommand::_Interrupted() { _End(); }

void PIDCommand::_End() { m_runner.Disable(); }

void PIDCommand::SetReferenceRelative(double deltaReference) {
  SetReference(GetReference() + deltaReference);
}

const PIDController& PIDCommand::GetPIDController() const {
  return m_controller;
}

void PIDCommand::SetReference(double reference) {
  m_controller.SetReference(reference);
}

double PIDCommand::GetReference() const { return m_controller.GetReference(); }
