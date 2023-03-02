// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/RobotController.h>
#include <frc/simulation/DifferentialDrivetrainSim.h>
#include <frc/simulation/EncoderSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/system/plant/LinearSystemId.h>

#include "Constants.h"

/**
 * Simulation controller for the drivetrain.
 */
class DriveSim {
 public:
  DriveSim();

  /**
   * Hack to work around lack of method pointer capturing.
   */
  static void CallSimulationPeriodic(void* self);

  /**
   * Call this to advance the simulation by 20 ms.
   */
  void SimulationPeriodic();

  /**
   * Get the duty cycle commanded to the left side of the drivetrain.
   */
  double GetLeftDutyCycle() const;

  /**
   * Get the duty cycle commanded to the right side of the drivetrain.
   */
  double GetRightDutyCycle() const;

  /**
   * Get the distance traveled by the left side of the drivetrain.
   */
  units::meter_t GetLeftDistance() const;

  /**
   * Get the distance traveled by the right side of the drivetrain.
   */
  units::meter_t GetRightDistance() const;

 private:
  frc::sim::PWMSim m_leftLeader{DriveConstants::kLeftMotor1Port};
  frc::sim::PWMSim m_leftFollower{DriveConstants::kLeftMotor2Port};
  frc::sim::PWMSim m_rightLeader{DriveConstants::kRightMotor1Port};
  frc::sim::PWMSim m_rightFollower{DriveConstants::kRightMotor2Port};

  frc::sim::EncoderSim m_leftEncoder;
  frc::sim::EncoderSim m_rightEncoder;
  frc::sim::DifferentialDrivetrainSim m_drive;
};
