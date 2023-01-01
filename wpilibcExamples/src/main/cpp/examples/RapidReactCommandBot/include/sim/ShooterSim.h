// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/RobotController.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/FlywheelSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/system/plant/LinearSystemId.h>

#include "Constants.h"

/**
 * Simulation controller for the shooter subsystem.
 */
class ShooterSim {
 public:
  ShooterSim();

  /**
   * Call this to advance the simulation by 20 ms.
   */
  void SimulationPeriodic();

  /**
   * Returns the duty cycle applied to the shooter motor.
   */
  double GetShooterMotor();

  /**
   * Returns the duty cycle applied to the feeder motor.
   */
  double GetFeederMotor();

 private:
  frc::sim::PWMSim m_shooterMotor;
  frc::sim::PWMSim m_feederMotor;
  frc::sim::EncoderSim m_encoder;
  frc::sim::FlywheelSim m_flywheel;
};
