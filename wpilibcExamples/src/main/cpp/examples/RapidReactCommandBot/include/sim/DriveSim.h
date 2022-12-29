// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.h"

#include <frc/RobotController.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/simulation/DifferentialDrivetrainSim.h>
#include <frc/system/plant/LinearSystemId.h>

class DriveSim {
 public:
  DriveSim();
  void SimulationPeriodic();

 private:
  frc::sim::PWMSim m_leftLeader{DriveConstants::kLeftMotor1Port};
  frc::sim::PWMSim m_leftFollower{DriveConstants::kLeftMotor2Port};
  frc::sim::PWMSim m_rightLeader{DriveConstants::kRightMotor1Port};
  frc::sim::PWMSim m_rightFollower{DriveConstants::kRightMotor2Port};

  frc::sim::EncoderSim m_leftEncoder;
  frc::sim::EncoderSim m_rightEncoder;
  frc::sim::DifferentialDrivetrainSim m_drive;
};
