// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Elevator.h"

#include <frc/RobotController.h>
#include <frc/StateSpaceUtil.h>
#include <frc/smartdashboard/SmartDashboard.h>

Elevator::Elevator() {
  m_encoder.SetDistancePerPulse(Constants::kArmEncoderDistPerPulse);

  // Put Mechanism 2d to SmartDashboard
  // To view the Elevator visualization, select Network Tables -> SmartDashboard
  // -> Elevator Sim
  frc::SmartDashboard::PutData("Elevator Sim", &m_mech2d);
}

void Elevator::SimulationPeriodic() {
  // In this method, we update our simulation of what our elevator is doing
  // First, we set our "inputs" (voltages)
  m_elevatorSim.SetInput(frc::Vectord<1>{
      m_motorSim.GetSpeed() * frc::RobotController::GetInputVoltage()});

  // Next, we update it. The standard loop time is 20ms.
  m_elevatorSim.Update(20_ms);

  // Finally, we set our simulated encoder's readings and simulated battery
  // voltage
  m_encoderSim.SetDistance(m_elevatorSim.GetPosition().value());
  // SimBattery estimates loaded battery voltages
  frc::sim::RoboRioSim::SetVInVoltage(
      frc::sim::BatterySim::Calculate({m_elevatorSim.GetCurrentDraw()}));
}

void Elevator::UpdateTelemetry() {
  // Update the Elevator length based on the simulated elevator height
  m_elevatorMech2d->SetLength(m_encoder.GetDistance());
}

void Elevator::ReachGoal(units::meter_t goal) {
  frc::ExponentialProfile<units::meters, units::volts>::State goalState{goal,
                                                                        0_mps};

  auto next = m_profile.Calculate(20_ms, m_setpoint, goalState);

  auto pidOutput = m_controller.Calculate(m_encoder.GetDistance(),
                                          m_setpoint.position / 1_m);
  auto feedforwardOutput =
      m_feedforward.Calculate(m_setpoint.velocity, next.velocity);

  m_motor.SetVoltage(units::volt_t{pidOutput} + feedforwardOutput);

  m_setpoint = next;
}

void Elevator::Reset() {
  m_setpoint = {m_encoder.GetDistance() * 1_m, 0_mps};
}

void Elevator::Stop() {
  m_motor.Set(0.0);
}
