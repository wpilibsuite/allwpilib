// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Arm.hpp"

#include "wpi/preferences/Preferences.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/telemetry/Telemetry.hpp"

Arm::Arm() {
  encoder.SetDistancePerPulse(ARM_ENCODER_DIST_PER_PULSE);

  // Set the Arm position setpoint and P constant to Preferences if the keys
  // don't already exist
  wpi::Preferences::InitDouble(ARM_POSITION_KEY, armSetpoint.value());
  wpi::Preferences::InitDouble(ARM_P_KEY, armKp);
}

void Arm::SimulationPeriodic() {
  // In this method, we update our simulation of what our arm is doing
  // First, we set our "inputs" (voltages)
  armSim.SetInput(wpi::math::Vectord<1>{
      motor.GetThrottle() * wpi::RobotController::GetInputVoltage()});

  // Next, we update it. The standard loop time is 20ms.
  armSim.Update(20_ms);

  // Finally, we set our simulated encoder's readings and simulated battery
  // voltage
  encoderSim.SetDistance(armSim.GetAngle().value());
  // SimBattery estimates loaded battery voltages
  wpi::sim::RoboRioSim::SetVInVoltage(
      wpi::sim::BatterySim::Calculate({armSim.GetCurrentDraw()}));

  // Update the Mechanism Arm angle based on the simulated arm angle
  arm->SetAngle(armSim.GetAngle());

  // Put Mechanism 2d to telemetry
  wpi::telemetry::Log("Arm Sim", mech2d);
}

void Arm::LoadPreferences() {
  // Read Preferences for Arm setpoint and kP on entering Teleop
  armSetpoint = wpi::units::degrees<>{
      wpi::Preferences::GetDouble(ARM_POSITION_KEY, armSetpoint.value())};
  if (armKp != wpi::Preferences::GetDouble(ARM_P_KEY, armKp)) {
    armKp = wpi::Preferences::GetDouble(ARM_P_KEY, armKp);
    controller.SetP(armKp);
  }
}

void Arm::ReachSetpoint() {
  // Here, we run PID control like normal, with a setpoint read from
  // preferences in degrees.
  double pidOutput = controller.Calculate(
      encoder.GetDistance(), (wpi::units::radians<>{armSetpoint}.value()));
  motor.SetVoltage(wpi::units::volts<>{pidOutput});
}

void Arm::Stop() {
  motor.SetThrottle(0.0);
}
