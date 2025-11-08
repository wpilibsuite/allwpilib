// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Elevator.hpp"

#include "wpi/system/RobotController.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

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
  m_controller.SetGoal(goal);
  // With the setpoint value we run PID control like normal
  double pidOutput =
      m_controller.Calculate(units::meter_t{m_encoder.GetDistance()});
  units::volt_t feedforwardOutput =
      m_feedforward.Calculate(m_controller.GetSetpoint().velocity);
  m_motor.SetVoltage(units::volt_t{pidOutput} + feedforwardOutput);
}

void Elevator::Stop() {
  m_controller.SetGoal(0.0_m);
  m_motor.Set(0.0);
}
