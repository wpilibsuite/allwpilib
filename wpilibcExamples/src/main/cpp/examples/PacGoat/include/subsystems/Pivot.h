/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/AnalogPotentiometer.h>
#include <frc/DigitalInput.h>
#include <frc/PWMVictorSPX.h>
#include <frc/commands/PIDSubsystem.h>

/**
 * The Pivot subsystem contains the Van-door motor and the pot for PID control
 * of angle of the pivot and claw.
 */
class Pivot : public frc::PIDSubsystem {
 public:
  // Constants for some useful angles
  static constexpr double kCollect = 105;
  static constexpr double kLowGoal = 90;
  static constexpr double kShoot = 45;
  static constexpr double kShootNear = 30;

  Pivot();

  /**
   *  No default command, if PID is enabled, the current setpoint will be
   * maintained.
   */
  void InitDefaultCommand() override {}

  /**
   * @return The angle read in by the potentiometer
   */
  double ReturnPIDInput() override;

  /**
   * Set the motor speed based off of the PID output
   */
  void UsePIDOutput(double output) override;

  /**
   * @return If the pivot is at its upper limit.
   */
  bool IsAtUpperLimit();

  /**
   * @return If the pivot is at its lower limit.
   */
  bool IsAtLowerLimit();

  /**
   * @return The current angle of the pivot.
   */
  double GetAngle();

 private:
  // Subsystem devices

  // Sensors for measuring the position of the pivot
  frc::DigitalInput m_upperLimitSwitch{13};
  frc::DigitalInput m_lowerLimitSwitch{12};

  /* 0 degrees is vertical facing up.
   * Angle increases the more forward the pivot goes.
   */
  frc::AnalogPotentiometer m_pot{1};

  // Motor to move the pivot
  frc::PWMVictorSPX m_motor{5};
};
