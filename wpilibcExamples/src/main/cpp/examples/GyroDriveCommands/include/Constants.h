/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
    (kWheelDiameterInches * 3.142) / static_cast<double>(kEncoderCPR);

constexpr bool kGyroReversed = true;

constexpr double kStabilizationP = 1;
constexpr double kStabilizationI = .5;
constexpr double kStabilizationD = 0;

constexpr double kTurnP = 1;
constexpr double kTurnI = 0;
constexpr double kTurnD = 0;

constexpr double kTurnToleranceDeg = 5;
constexpr double kTurnRateToleranceDegPerS = 10;  // degrees per second
}  // namespace DriveConstants

namespace AutoConstants {
constexpr double kAutoDriveDistanceInches = 60;
constexpr double kAutoBackupDistanceInches = 20;
constexpr double kAutoDriveSpeed = .5;
}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 1;
}  // namespace OIConstants
