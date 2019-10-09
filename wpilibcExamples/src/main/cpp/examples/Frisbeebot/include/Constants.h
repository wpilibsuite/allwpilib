/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
const int kLeftMotor1Port = 0;
const int kLeftMotor2Port = 1;
const int kRightMotor1Port = 2;
const int kRightMotor2Port = 3;

const int kLeftEncoderPorts[]{0, 1};
const int kRightEncoderPorts[]{2, 3};
const bool kLeftEncoderReversed = false;
const bool kRightEncoderReversed = true;

const int kEncoderCPR = 1024;
const double kWheelDiameterInches = 6;
const double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * 3.142) / static_cast<double>(kEncoderCPR);
}  // namespace DriveConstants

namespace ShooterConstants {
const int kEncoderPorts[]{4, 5};
const bool kEncoderReversed = false;
const int kEncoderCPR = 1024;
const double kEncoderDistancePerPulse =
    // Distance units will be rotations
    1. / static_cast<double>(kEncoderCPR);

const int kShooterMotorPort = 4;
const int kFeederMotorPort = 5;

const double kShooterFreeRPS = 5300;
const double kShooterTargetRPS = 4000;
const double kShooterToleranceRPS = 50;

const double kP = 1;
const double kI = 0;
const double kD = 0;

// On a real robot the feedforward constants should be empirically determined;
// these are reasonable guesses.
const double kSFractional = .05;
const double kVFractional =
    // Should have value 1 at free speed...
    1. / kShooterFreeRPS;

const double kFeederSpeed = .5;
}  // namespace ShooterConstants

namespace AutoConstants {
constexpr auto kAutoTimeoutSeconds = 12_s;
constexpr auto kAutoShootTimeSeconds = 7_s;
}  // namespace AutoConstants

namespace OIConstants {
const int kDriverControllerPort = 1;
}  // namespace OIConstants
