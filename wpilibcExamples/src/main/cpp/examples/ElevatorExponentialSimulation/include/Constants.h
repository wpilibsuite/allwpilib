// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace Constants {

inline constexpr int MOTOR_PORT = 0;
inline constexpr int ENCODER_A_CHANNEL = 0;
inline constexpr int ENCODER_B_CHANNEL = 1;
inline constexpr int JOYSTICK_PORT = 0;

inline constexpr double ELEVATOR_P = 0.75;
inline constexpr double ELEVATOR_I = 0.0;
inline constexpr double ELEVATOR_D = 0.0;

inline constexpr units::volt_t ELEVATOR_MAX_V = 10_V;
inline constexpr units::volt_t ELEVATOR_S = 0.0_V;
inline constexpr units::volt_t ELEVATOR_G = 0.62_V;
inline constexpr auto ELEVATOR_V = 3.9_V / 1_mps;
inline constexpr auto ELEVATOR_A = 0.06_V / 1_mps_sq;

inline constexpr double ELEVATOR_GEARING = 5.0;
inline constexpr units::meter_t ELEVATOR_DRUM_RADIUS = 1_in;
inline constexpr units::kilogram_t CARRIAGE_MASS = 12_lb;

inline constexpr units::meter_t SETPOINT = 42.875_in;
inline constexpr units::meter_t LOWER_SETPOINT = 15_in;
inline constexpr units::meter_t MIN_ELEVATOR_HEIGHT = 0_cm;
inline constexpr units::meter_t MAX_ELEVATOR_HEIGHT = 50_in;

// distance per pulse = (distance per revolution) / (pulses per revolution)
//  = (Pi * D) / ppr
inline constexpr double ARM_ENCODER_DIST_PER_PULSE =
    2.0 * std::numbers::pi * ELEVATOR_DRUM_RADIUS.value() / 4096.0;

}  // namespace Constants
