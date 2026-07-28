// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Arm.hpp"

#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/Preferences.hpp"

Arm::Arm() {
  encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);

  // Put Mechanism 2d to SmartDashboard
  wpi::SmartDashboard::PutData("Arm Sim", &mech2d);

  // Set the Arm position setpoint and P constant to Preferences if the keys
  // don't already exist
  wpi::Preferences::InitDouble(kArmPositionKey, armSetpoint.value());
  wpi::Preferences::InitDouble(kArmPKey, armKp);
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
}

void Arm::LoadPreferences() {
  // Read Preferences for Arm setpoint and kP on entering Teleop
  armSetpoint = wpi::units::degree_t{
      wpi::Preferences::GetDouble(kArmPositionKey, armSetpoint.value())};
  if (armKp != wpi::Preferences::GetDouble(kArmPKey, armKp)) {
    armKp = wpi::Preferences::GetDouble(kArmPKey, armKp);
    controller.SetP(armKp);
  }
}

void Arm::ReachSetpoint() {
  // Here, we run PID control like normal, with a setpoint read from
  // preferences in degrees.
  double pidOutput = controller.Calculate(
      encoder.GetDistance(), (wpi::units::radian_t{armSetpoint}.value()));
  motor.SetVoltage(wpi::units::volt_t{pidOutput});
}

void Arm::Stop() {
  motor.SetThrottle(0.0);
}
