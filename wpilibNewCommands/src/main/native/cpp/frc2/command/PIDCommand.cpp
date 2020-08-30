/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
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
                       std::function<double()> setpointSource,
                       std::function<void(double)> useOutput,
                       wpi::ArrayRef<Subsystem*> requirements)
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
    : PIDCommand(
          controller, measurementSource, [setpoint] { return setpoint; },
          useOutput, requirements) {}

PIDCommand::PIDCommand(PIDController controller,
                       std::function<double()> measurementSource,
                       double setpoint, std::function<void(double)> useOutput,
                       wpi::ArrayRef<Subsystem*> requirements)
    : PIDCommand(
          controller, measurementSource, [setpoint] { return setpoint; },
          useOutput, requirements) {}

void PIDCommand::Initialize() { m_controller.Reset(); }

void PIDCommand::Execute() {
  m_useOutput(m_controller.Calculate(m_measurement(), m_setpoint()));
}

void PIDCommand::End(bool interrupted) { m_useOutput(0); }

PIDController& PIDCommand::GetController() { return m_controller; }
