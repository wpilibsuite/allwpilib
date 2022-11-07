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
constexpr int kLeftMotor1Port = 0;
constexpr int kLeftMotor2Port = 1;
constexpr int kRightMotor1Port = 2;
constexpr int kRightMotor2Port = 3;

constexpr int kLeftEncoderPorts[]{0, 1};
constexpr int kRightEncoderPorts[]{2, 3};
constexpr bool kLeftEncoderReversed = false;
constexpr bool kRightEncoderReversed = true;

constexpr int kEncoderCPR = 1024;
constexpr double kWheelDiameterInches = 6;
constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * std::numbers::pi) /
    static_cast<double>(kEncoderCPR);
}  // namespace DriveConstants

namespace ShooterConstants {
constexpr int kEncoderPorts[]{4, 5};
constexpr bool kEncoderReversed = false;
constexpr int kEncoderCPR = 1024;
constexpr double kEncoderDistancePerPulse =
    // Distance units will be rotations
    1.0 / static_cast<double>(kEncoderCPR);

constexpr int kShooterMotorPort = 4;
constexpr int kFeederMotorPort = 5;

constexpr auto kShooterFreeRPS = 5300_tr / 1_s;
constexpr auto kShooterTargetRPS = 4000_tr / 1_s;
constexpr auto kShooterToleranceRPS = 50_tr / 1_s;

constexpr double kP = 1;
constexpr double kI = 0;
constexpr double kD = 0;

// On a real robot the feedforward constants should be empirically determined;
// these are reasonable guesses.
constexpr auto kS = 0.05_V;
constexpr auto kV =
    // Should have value 12V at free speed...
    12_V / kShooterFreeRPS;

constexpr double kFeederSpeed = 0.5;
}  // namespace ShooterConstants

namespace AutoConstants {
constexpr auto kAutoTimeoutSeconds = 12_s;
constexpr auto kAutoShootTimeSeconds = 7_s;
}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants
