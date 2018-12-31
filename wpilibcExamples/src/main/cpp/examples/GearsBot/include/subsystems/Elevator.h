/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/AnalogPotentiometer.h>
#include <frc/PWMVictorSPX.h>
#include <frc/commands/PIDSubsystem.h>

/**
 * The elevator subsystem uses PID to go to a given height. Unfortunately, in
 * it's current
 * state PID values for simulation are different than in the real world do to
 * minor differences.
 */
class Elevator : public frc::PIDSubsystem {
 public:
  Elevator();

  void InitDefaultCommand() override;

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
  void Log();

  /**
   * Use the potentiometer as the PID sensor. This method is automatically
   * called by the subsystem.
   */
  double ReturnPIDInput() override;

  /**
   * Use the motor as the PID output. This method is automatically called
   * by
   * the subsystem.
   */
  void UsePIDOutput(double d) override;

 private:
  frc::PWMVictorSPX m_motor{5};

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
