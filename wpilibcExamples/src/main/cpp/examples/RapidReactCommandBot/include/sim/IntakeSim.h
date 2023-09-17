// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/simulation/DoubleSolenoidSim.h>
#include <frc/simulation/PWMSim.h>

#include "Constants.h"

/**
 * Simulation controller for the intake.
 */
class IntakeSim {
 public:
  /**
   * Call this to advance the simulation by 20 ms.
   */
  void SimulationPeriodic();

  /**
   * Get the duty cycle commanded to the intake motor.
   */
  double GetMotor() const;

  /**
   * Is the intake deployed.
   */
  bool IsDeployed() const;

 private:
  frc::sim::PWMSim m_motor{IntakeConstants::kMotorPort};
  frc::sim::DoubleSolenoidSim m_piston{frc::PneumaticsModuleType::CTREPCM,
                               IntakeConstants::kSolenoidPorts[0],
                               IntakeConstants::kSolenoidPorts[1]};
};
