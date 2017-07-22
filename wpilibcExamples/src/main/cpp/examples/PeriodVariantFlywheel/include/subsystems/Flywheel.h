/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>

#include <Eigen/Core>
#include <frc/Encoder.h>
#include <frc/Notifier.h>
#include <frc/Spark.h>

#include "subsystems/FlywheelController.h"

class Flywheel {
 public:
  Flywheel();

  Flywheel(const Flywheel&) = delete;
  Flywheel& operator=(const Flywheel&) = delete;

  /**
   * Enable controller.
   */
  void Enable();

  /**
   * Disable controller.
   */
  void Disable();

  void SetReference(double angularVelocity);

  bool AtReference() const;

  /**
   * Iterates the shooter control loop one cycle.
   */
  void Iterate();

  void Reset();

 private:
  frc::Spark m_motor{1};
  frc::Encoder m_encoder{1, 2};

  FlywheelController m_wheel;
  frc::Notifier m_thread{&Flywheel::Iterate, this};
  std::chrono::steady_clock::time_point m_lastTime =
      std::chrono::steady_clock::time_point::min();
};
