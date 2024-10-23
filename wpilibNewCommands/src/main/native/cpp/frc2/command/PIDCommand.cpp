// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/PIDCommand.h"

#include <utility>

#include <wpi/deprecated.h>

using namespace frc2;

PIDCommand::PIDCommand(frc::PIDController controller,
                       std::function<double()> measurementSource,
                       std::function<double()> setpointSource,
                       std::function<void(double)> useOutput,
                       Requirements requirements)
    : m_controller{std::move(controller)},
      m_measurement{std::move(measurementSource)},
      m_setpoint{std::move(setpointSource)},
      m_useOutput{std::move(useOutput)} {
  AddRequirements(requirements);
}
WPI_IGNORE_DEPRECATED

PIDCommand::PIDCommand(frc::PIDController controller,
                       std::function<double()> measurementSource,
                       double setpoint, std::function<void(double)> useOutput,
                       Requirements requirements)
    : PIDCommand(
          controller, measurementSource, [setpoint] { return setpoint; },
          useOutput, requirements) {}
WPI_UNIGNORE_DEPRECATED

void PIDCommand::Initialize() {
  m_controller.Reset();
}

void PIDCommand::Execute() {
  m_useOutput(m_controller.Calculate(m_measurement(), m_setpoint()));
}

void PIDCommand::End(bool interrupted) {
  m_useOutput(0);
}

frc::PIDController& PIDCommand::GetController() {
  return m_controller;
}
