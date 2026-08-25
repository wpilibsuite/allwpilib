// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

inline constexpr int MOTOR_PORT = 0;
inline constexpr int ENCODER_A_CHANNEL = 0;
inline constexpr int ENCODER_B_CHANNEL = 1;
inline constexpr int JOYSTICK_PORT = 0;

inline constexpr std::string_view ARM_POSITION_KEY = "ArmPosition";
inline constexpr std::string_view ARM_P_KEY = "ArmP";

inline constexpr double DEFAULT_ARM_KP = 50.0;
inline constexpr wpi::units::degrees<> DEFAULT_ARM_SETPOINT = 75.0_deg;

inline constexpr wpi::units::radians<> MIN_ANGLE = -75.0_deg;
inline constexpr wpi::units::radians<> MAX_ANGLE = 255.0_deg;

inline constexpr double ARM_REDUCTION = 200.0;
inline constexpr wpi::units::kilograms<> ARM_MASS = 8.0_kg;
inline constexpr wpi::units::meters<> ARM_LENGTH = 30.0_in;

// distance per pulse = (angle per revolution) / (pulses per revolution)
//  = (2 * PI rads) / (4096 pulses)
inline constexpr double ARM_ENCODER_DIST_PER_PULSE =
    2.0 * std::numbers::pi / 4096.0;
