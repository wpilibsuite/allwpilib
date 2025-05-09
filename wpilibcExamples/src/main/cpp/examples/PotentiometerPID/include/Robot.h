// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <frc/AnalogPotentiometer.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <units/length.h>

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopInit() override;
  void TeleopPeriodic() override;

  static constexpr int POT_CHANNEL = 1;
  static constexpr int MOTOR_CHANNEL = 7;
  static constexpr int JOYSTICK_CHANNEL = 3;

  // The elevator can move 1.5 meters from top to bottom
  static constexpr units::meter_t FULL_HEIGHT = 1.5_m;

  // Bottom, middle, and top elevator setpoints
  static constexpr std::array<units::meter_t, 3> SETPOINTS = {
      {0.2_m, 0.8_m, 1.4_m}};

 private:
  // proportional speed constant
  // negative because applying positive voltage will bring us closer to the
  // target
  static constexpr double kP = 0.7;
  // integral speed constant
  static constexpr double kI = 0.35;
  // derivative speed constant
  static constexpr double kD = 0.25;

  // Scaling is handled internally
  frc::AnalogPotentiometer m_potentiometer{POT_CHANNEL, FULL_HEIGHT.value()};

  frc::PWMSparkMax m_elevatorMotor{MOTOR_CHANNEL};
  frc::PIDController m_pidController{kP, kI, kD};
  frc::Joystick m_joystick{JOYSTICK_CHANNEL};

  size_t m_index;
};
