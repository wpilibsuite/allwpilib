// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
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

namespace Constants {

inline constexpr int kMotorPort = 0;
inline constexpr int kEncoderAChannel = 0;
inline constexpr int kEncoderBChannel = 1;
inline constexpr int kJoystickPort = 0;

inline constexpr double kElevatorKp = 0.75;
inline constexpr double kElevatorKi = 0.0;
inline constexpr double kElevatorKd = 0.0;

inline constexpr wpi::units::volts<> kElevatorMaxV = 10_V;
inline constexpr wpi::units::volts<> kElevatorkS = 0.0_V;
inline constexpr wpi::units::volts<> kElevatorkG = 0.62_V;
inline constexpr auto kElevatorkV = 3.9_V / 1_mps;
inline constexpr auto kElevatorkA = 0.06_V / 1_mps2;

inline constexpr double kElevatorGearing = 5.0;
inline constexpr wpi::units::meters<> kElevatorDrumRadius = 1_in;
inline constexpr wpi::units::kilograms<> kCarriageMass = 12_lb;

inline constexpr wpi::units::meters<> kSetpoint = 42.875_in;
inline constexpr wpi::units::meters<> kLowerSetpoint = 15_in;
inline constexpr wpi::units::meters<> kMinElevatorHeight = 0_cm;
inline constexpr wpi::units::meters<> kMaxElevatorHeight = 50_in;

// distance per pulse = (distance per revolution) / (pulses per revolution)
//  = (Pi * D) / ppr
inline constexpr double kArmEncoderDistPerPulse =
    2.0 * std::numbers::pi * kElevatorDrumRadius.value() / 4096.0;

}  // namespace Constants
