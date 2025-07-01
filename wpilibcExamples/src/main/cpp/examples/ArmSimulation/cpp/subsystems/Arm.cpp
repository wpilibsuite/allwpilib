// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Arm.hpp"

#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/util/Preferences.hpp"

Arm::Arm() {
  m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);

  // Set the Arm position setpoint and P constant to Preferences if the keys
  // don't already exist
  wpi::Preferences::InitDouble(kArmPositionKey, m_armSetpoint.value());
  wpi::Preferences::InitDouble(kArmPKey, m_armKp);
}

void Arm::SimulationPeriodic() {
  // In this method, we update our simulation of what our arm is doing
  // First, we set our "inputs" (voltages)
  m_armSim.SetInput(wpi::math::Vectord<1>{
      m_motor.Get() * wpi::RobotController::GetInputVoltage()});

  // Next, we update it. The standard loop time is 20ms.
  m_armSim.Update(20_ms);

  // Finally, we set our simulated encoder's readings and simulated battery
  // voltage
  m_encoderSim.SetDistance(m_armSim.GetAngle().value());
  // SimBattery estimates loaded battery voltages
  wpi::sim::RoboRioSim::SetVInVoltage(
      wpi::sim::BatterySim::Calculate({m_armSim.GetCurrentDraw()}));

  // Update the Mechanism Arm angle based on the simulated arm angle
  m_arm->SetAngle(m_armSim.GetAngle());

  // Put Mechanism 2d to Telemetry
  wpi::Telemetry::Log("Arm Sim", m_mech2d);
}

void Arm::LoadPreferences() {
  // Read Preferences for Arm setpoint and kP on entering Teleop
  m_armSetpoint = wpi::units::degree_t{
      wpi::Preferences::GetDouble(kArmPositionKey, m_armSetpoint.value())};
  if (m_armKp != wpi::Preferences::GetDouble(kArmPKey, m_armKp)) {
    m_armKp = wpi::Preferences::GetDouble(kArmPKey, m_armKp);
    m_controller.SetP(m_armKp);
  }
}

void Arm::ReachSetpoint() {
  // Here, we run PID control like normal, with a setpoint read from
  // preferences in degrees.
  double pidOutput = m_controller.Calculate(
      m_encoder.GetDistance(), (wpi::units::radian_t{m_armSetpoint}.value()));
  m_motor.SetVoltage(wpi::units::volt_t{pidOutput});
}

void Arm::Stop() {
  m_motor.Set(0.0);
}
