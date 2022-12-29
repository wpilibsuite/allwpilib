// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sim/DriveSim.h"

DriveSim::DriveSim()
  :
  m_leftEncoder{frc::sim::EncoderSim::CreateForChannel(DriveConstants::kLeftEncoderPorts[0])},
  m_rightEncoder{frc::sim::EncoderSim::CreateForChannel(DriveConstants::kRightEncoderPorts[0])},
  m_drive{
    frc::LinearSystemId::IdentifyDrivetrainSystem(
      DriveConstants::kv, DriveConstants::ka,
      DriveConstants::kvAngular, DriveConstants::kaAngular),
    DriveConstants::kTrackwidth,
    frc::DCMotor::NEO(2),
    frc::sim::DifferentialDrivetrainSim::KitbotGearing::k10p71,
    DriveConstants::kWheelDiameter / 2.0}
{}

void DriveSim::SimulationPeriodic() {
  auto battery = frc::RobotController::GetBatteryVoltage();
  m_drive.SetInputs(battery * m_leftLeader.GetSpeed(), battery * m_rightLeader.GetSpeed());

  m_drive.Update(20_ms);

  m_leftEncoder.SetDistance(m_drive.GetLeftPosition().value());
  m_rightEncoder.SetDistance(-m_drive.GetRightPosition().value());
}