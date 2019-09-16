/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/PIDCommand.h"

using namespace frc2;

PIDCommand::PIDCommand(PIDController controller,
                       std::function<double()> measurementSource,
                       std::function<double()> setpointSource,
                       std::function<void(double)> useOutput,
                       std::initializer_list<Subsystem*> requirements)
    : m_controller{controller},
      m_measurement{std::move(measurementSource)},
      m_setpoint{std::move(setpointSource)},
      m_useOutput{std::move(useOutput)} {
  AddRequirements(requirements);
}

PIDCommand::PIDCommand(PIDController controller,
                       std::function<double()> measurementSource,
                       double setpoint, std::function<void(double)> useOutput,
                       std::initializer_list<Subsystem*> requirements)
    : PIDCommand(controller, measurementSource, [setpoint] { return setpoint; },
                 useOutput, requirements) {}

void PIDCommand::Initialize() { m_controller.Reset(); }

void PIDCommand::Execute() {
  UseOutput(m_controller.Calculate(GetMeasurement(), m_setpoint()));
}

void PIDCommand::End(bool interrupted) { UseOutput(0); }

void PIDCommand::SetOutput(std::function<void(double)> useOutput) {
  m_useOutput = useOutput;
}

void PIDCommand::SetSetpoint(std::function<double()> setpointSource) {
  m_setpoint = setpointSource;
}

void PIDCommand::SetSetpoint(double setpoint) {
  m_setpoint = [setpoint] { return setpoint; };
}

void PIDCommand::SetSetpointRelative(double relativeSetpoint) {
  SetSetpoint(m_setpoint() + relativeSetpoint);
}

double PIDCommand::GetMeasurement() { return m_measurement(); }

void PIDCommand::UseOutput(double output) { m_useOutput(output); }

PIDController& PIDCommand::getController() { return m_controller; }
