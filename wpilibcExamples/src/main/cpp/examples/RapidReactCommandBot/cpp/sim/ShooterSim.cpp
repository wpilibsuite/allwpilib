// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sim/ShooterSim.h"

ShooterSim::ShooterSim()
    : m_shooterMotor{ShooterConstants::kShooterMotorPort},
      m_encoder{frc::sim::EncoderSim::CreateForChannel(
          ShooterConstants::kEncoderPorts[0])},
      m_flywheel{frc::LinearSystemId::IdentifyVelocitySystem<units::radians>(
                     ShooterConstants::kV, ShooterConstants::kA),
                 frc::DCMotor::NEO(), 1.0}
{}


void ShooterSim::SimulationPeriodic() {
    m_flywheel.SetInputVoltage(frc::RobotController::GetBatteryVoltage() * m_shooterMotor.GetSpeed());

    m_flywheel.Update(20_ms);

    units::turns_per_second_t rate = m_flywheel.GetAngularVelocity();
    m_encoder.SetRate(rate.value());
}