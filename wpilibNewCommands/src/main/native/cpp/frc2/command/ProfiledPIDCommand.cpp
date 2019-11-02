/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/ProfiledPIDCommand.h"

using namespace frc2;
using State = frc::TrapezoidProfile::State;

ProfiledPIDCommand::ProfiledPIDCommand(
    frc::ProfiledPIDController controller,
    std::function<units::meter_t()> measurementSource,
    std::function<State()> goalSource,
    std::function<void(double, State)> useOutput,
    std::initializer_list<Subsystem*> requirements)
    : m_controller{controller},
      m_measurement{std::move(measurementSource)},
      m_goal{std::move(goalSource)},
      m_useOutput{std::move(useOutput)} {
  AddRequirements(requirements);
}

ProfiledPIDCommand::ProfiledPIDCommand(
    frc::ProfiledPIDController controller,
    std::function<units::meter_t()> measurementSource,
    std::function<units::meter_t()> goalSource,
    std::function<void(double, State)> useOutput,
    std::initializer_list<Subsystem*> requirements)
    : ProfiledPIDCommand(controller, measurementSource,
                         [&goalSource]() {
                           return State{goalSource(), 0_mps};
                         },
                         useOutput, requirements) {}

ProfiledPIDCommand::ProfiledPIDCommand(
    frc::ProfiledPIDController controller,
    std::function<units::meter_t()> measurementSource, State goal,
    std::function<void(double, State)> useOutput,
    std::initializer_list<Subsystem*> requirements)
    : ProfiledPIDCommand(controller, measurementSource, [goal] { return goal; },
                         useOutput, requirements) {}

ProfiledPIDCommand::ProfiledPIDCommand(
    frc::ProfiledPIDController controller,
    std::function<units::meter_t()> measurementSource, units::meter_t goal,
    std::function<void(double, State)> useOutput,
    std::initializer_list<Subsystem*> requirements)
    : ProfiledPIDCommand(controller, measurementSource, [goal] { return goal; },
                         useOutput, requirements) {}

void ProfiledPIDCommand::Initialize() { m_controller.Reset(); }

void ProfiledPIDCommand::Execute() {
  m_useOutput(m_controller.Calculate(m_measurement(), m_goal()),
              m_controller.GetSetpoint());
}

void ProfiledPIDCommand::End(bool interrupted) {
  m_useOutput(0, State{0_m, 0_mps});
}

frc::ProfiledPIDController& ProfiledPIDCommand::GetController() {
  return m_controller;
}
