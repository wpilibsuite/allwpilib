// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.h"

#include <frc/RobotController.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/simulation/FlywheelSim.h>
#include <frc/system/plant/LinearSystemId.h>

class ShooterSim {
 public:
  ShooterSim();
  void SimulationPeriodic();

 private:
  frc::sim::PWMSim m_shooterMotor;
  frc::sim::EncoderSim m_encoder;
  frc::sim::FlywheelSim m_flywheel;
};
