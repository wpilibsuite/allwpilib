// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Elevator.hpp"

#include "wpi/system/RobotController.hpp"
#include "wpi/telemetry/Telemetry.hpp"

Elevator::Elevator() {
  encoder.SetDistancePerPulse(Constants::ARM_ENCODER_DIST_PER_PULSE);
}

void Elevator::SimulationPeriodic() {
  // In this method, we update our simulation of what our elevator is doing
  // First, we set our "inputs" (voltages)
  elevatorSim.SetInput(wpi::math::Vectord<1>{
      motorSim.GetThrottle() * wpi::RobotController::GetInputVoltage()});

  // Next, we update it. The standard loop time is 20ms.
  elevatorSim.Update(20_ms);

  // Finally, we set our simulated encoder's readings and simulated battery
  // voltage
  encoderSim.SetDistance(elevatorSim.GetPosition().value());
  // SimBattery estimates loaded battery voltages
  wpi::sim::RoboRioSim::SetVInVoltage(
      wpi::sim::BatterySim::Calculate({elevatorSim.GetCurrentDraw()}));
}

void Elevator::UpdateTelemetry() {
  // Update the Elevator length based on the simulated elevator height
  elevatorMech2d->SetLength(encoder.GetDistance());

  // Put Mechanism 2d to telemetry
  // To view the Elevator visualization, select Network Tables ->
  // Telemetry
  // -> Elevator Sim
  wpi::telemetry::Log("Elevator Sim", mech2d);
}

void Elevator::ReachGoal(wpi::units::meters<> goal) {
  controller.SetGoal(goal);
  // With the setpoint value we run PID control like normal
  double pidOutput =
      controller.Calculate(wpi::units::meters<>{encoder.GetDistance()});
  wpi::units::volts<> feedforwardOutput =
      feedforward.Calculate(controller.GetSetpoint().velocity);
  motor.SetVoltage(wpi::units::volts<>{pidOutput} + feedforwardOutput);
}

void Elevator::Stop() {
  controller.SetGoal(0.0_m);
  motor.SetThrottle(0.0);
}
