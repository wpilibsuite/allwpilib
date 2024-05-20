// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <units/angle.h>
#include <units/time.h>
#include <units/voltage.h>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
inline constexpr int kLeftMotor1Port = 0;
inline constexpr int kLeftMotor2Port = 1;
inline constexpr int kRightMotor1Port = 2;
inline constexpr int kRightMotor2Port = 3;

inline constexpr int kLeftEncoderPorts[]{0, 1};
inline constexpr int kRightEncoderPorts[]{2, 3};
inline constexpr bool kLeftEncoderReversed = false;
inline constexpr bool kRightEncoderReversed = true;

inline constexpr int kEncoderCPR = 1024;
inline constexpr double kWheelDiameterInches = 6;
inline constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * std::numbers::pi) /
    static_cast<double>(kEncoderCPR);
}  // namespace DriveConstants

namespace ShooterConstants {
inline constexpr int kEncoderPorts[]{4, 5};
inline constexpr bool kEncoderReversed = false;
inline constexpr int kEncoderCPR = 1024;
inline constexpr double kEncoderDistancePerPulse =
    // Distance units will be rotations
    1.0 / static_cast<double>(kEncoderCPR);

inline constexpr int kShooterMotorPort = 4;
inline constexpr int kFeederMotorPort = 5;

inline constexpr auto kShooterFreeRPS = 5300_tr / 1_s;
inline constexpr auto kShooterTargetRPS = 4000_tr / 1_s;
inline constexpr auto kShooterToleranceRPS = 50_tr / 1_s;

inline constexpr double kP = 1;
inline constexpr double kI = 0;
inline constexpr double kD = 0;

// On a real robot the feedforward constants should be empirically determined;
// these are reasonable guesses.
inline constexpr auto kS = 0.05_V;
inline constexpr auto kV =
    // Should have value 12V at free speed...
    12_V / kShooterFreeRPS;

inline constexpr double kFeederSpeed = 0.5;
}  // namespace ShooterConstants

namespace AutoConstants {
inline constexpr auto kAutoTimeoutSeconds = 12_s;
inline constexpr auto kAutoShootTimeSeconds = 7_s;
}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants
