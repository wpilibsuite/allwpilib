// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/simulation/DIOSim.h>
#include <frc/simulation/PWMSim.h>

#include "Constants.h"

/**
 * Simulation controller for the storage.
 */
class StorageSim {
 public:
  /**
   * Call this to advance the simulation by 20 ms.
   */
  void SimulationPeriodic();

  /**
   * Get the duty cycle commanded to the storage motor.
   */
  double GetMotor() const;

  /**
   * Set whether the storage is currently full.
   *
   * @param full true for full.
   */
  void SetIsFull(bool full);

 private:
  frc::sim::PWMSim m_motor{StorageConstants::kMotorPort};
  frc::sim::DIOSim m_ballSensor{StorageConstants::kBallSensorPort};
};
