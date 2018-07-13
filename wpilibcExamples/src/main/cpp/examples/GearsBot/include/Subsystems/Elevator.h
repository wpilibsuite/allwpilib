/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <AnalogPotentiometer.h>
#include <Commands/PIDSubsystem.h>
#include <Spark.h>

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
  frc::Spark m_motor{5};

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
