// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogPotentiometer.h>
#include <frc/PWMVictorSPX.h>
#include <frc2/command/PIDSubsystem.h>

/**
 * The elevator subsystem uses PID to go to a given height. Unfortunately, in
 * it's current
 * state PID values for simulation are different than in the real world do to
 * minor differences.
 */
class Elevator : public frc2::PIDSubsystem {
 public:
  Elevator();

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
  void Log();

  /**
   * Use the potentiometer as the PID sensor. This method is automatically
   * called by the subsystem.
   */
  double GetMeasurement() override;

  /**
   * Use the motor as the PID output. This method is automatically called
   * by
   * the subsystem.
   */
  void UseOutput(double output, double setpoint) override;

  /**
   * Log the data periodically. This method is automatically called
   * by the subsystem.
   */
  void Periodic() override;

 private:
  frc::PWMVictorSPX m_motor{5};
  double m_setpoint = 0;

// Conversion value of potentiometer varies between the real world and
// simulation
#ifndef SIMULATION
  frc::AnalogPotentiometer m_pot{2, -2.0 / 5};
#else
  frc::AnalogPotentiometer m_pot{2};  // Defaults to meters
#endif

  static constexpr double kP_real = 4;
  static constexpr double kI_real = 0.07;
  static constexpr double kP_simulation = 18;
  static constexpr double kI_simulation = 0.2;
};
