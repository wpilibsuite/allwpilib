// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Arm.h"

#include <frc/Preferences.h>
#include <frc/RobotController.h>
#include <frc/StateSpaceUtil.h>
#include <frc/smartdashboard/SmartDashboard.h>

Arm::Arm() {
  m_encoder.SetDistancePerPulse(kArmEncoderDistPerPulse);

  // Put Mechanism 2d to SmartDashboard
  frc::SmartDashboard::PutData("Arm Sim", &m_mech2d);

  // Set the Arm position setpoint and P constant to Preferences if the keys
  // don't already exist
  frc::Preferences::InitDouble(kArmPositionKey, m_armSetpoint.value());
  frc::Preferences::InitDouble(kArmPKey, m_armKp);
}

void Arm::SimulationPeriodic() {
  // In this method, we update our simulation of what our arm is doing
  // First, we set our "inputs" (voltages)
  m_armSim.SetInput(
      frc::Vectord<1>{m_motor.Get() * frc::RobotController::GetInputVoltage()});

  // Next, we update it. The standard loop time is 20ms.
  m_armSim.Update(20_ms);

  // Finally, we set our simulated encoder's readings and simulated battery
  // voltage
  m_encoderSim.SetDistance(m_armSim.GetAngle().value());
  // SimBattery estimates loaded battery voltages
  frc::sim::RoboRioSim::SetVInVoltage(
      frc::sim::BatterySim::Calculate({m_armSim.GetCurrentDraw()}));

  // Update the Mechanism Arm angle based on the simulated arm angle
  m_arm->SetAngle(m_armSim.GetAngle());
}

void Arm::LoadPreferences() {
  // Read Preferences for Arm setpoint and kP on entering Teleop
  m_armSetpoint = units::degree_t{
      frc::Preferences::GetDouble(kArmPositionKey, m_armSetpoint.value())};
  if (m_armKp != frc::Preferences::GetDouble(kArmPKey, m_armKp)) {
    m_armKp = frc::Preferences::GetDouble(kArmPKey, m_armKp);
    m_controller.SetP(m_armKp);
  }
}

void Arm::ReachSetpoint() {
  // Here, we run PID control like normal, with a setpoint read from
  // preferences in degrees.
  double pidOutput = m_controller.Calculate(
      m_encoder.GetDistance(), (units::radian_t{m_armSetpoint}.value()));
  m_motor.SetVoltage(units::volt_t{pidOutput});
}

void Arm::Stop() {
  m_motor.Set(0.0);
}
