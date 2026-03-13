// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/units/length.hpp"

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
class Robot : public wpi::TimedRobot {
 public:
  void TeleopInit() override;
  void TeleopPeriodic() override;

  static constexpr int kPotChannel = 1;
  static constexpr int kMotorChannel = 7;
  static constexpr int kJoystickChannel = 3;

  // The elevator can move 1.5 meters from top to bottom
  static constexpr wpi::units::meter_t kFullHeight = 1.5_m;

  // Bottom, middle, and top elevator setpoints
  static constexpr std::array<wpi::units::meter_t, 3> kSetpoints = {
      {0.2_m, 0.8_m, 1.4_m}};

 private:
  // proportional velocity constant
  // negative because applying positive voltage will bring us closer to the
  // target
  static constexpr double kP = 0.7;
  // integral velocity constant
  static constexpr double kI = 0.35;
  // derivative velocity constant
  static constexpr double kD = 0.25;

  // Scaling is handled internally
  wpi::AnalogPotentiometer m_potentiometer{kPotChannel, kFullHeight.value()};

  wpi::PWMSparkMax m_elevatorMotor{kMotorChannel};
  wpi::math::PIDController m_pidController{kP, kI, kD};
  wpi::Joystick m_joystick{kJoystickChannel};

  size_t m_index;
};
