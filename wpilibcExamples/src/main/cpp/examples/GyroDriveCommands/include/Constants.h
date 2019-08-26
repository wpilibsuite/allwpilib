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

const bool kGyroReversed = false;

const double kStabilizationP = 1;
const double kStabilizationI = .5;
const double kStabilizationD = 0;

const double kTurnP = 1;
const double kTurnI = 0;
const double kTurnD = 0;

const double kTurnToleranceDeg = 5;
const double kTurnRateToleranceDegPerS = 10;  // degrees per second
}  // namespace DriveConstants

namespace AutoConstants {
const double kAutoDriveDistanceInches = 60;
const double kAutoBackupDistanceInches = 20;
const double kAutoDriveSpeed = .5;
}  // namespace AutoConstants

namespace OIConstants {
const int kDriverControllerPort = 1;
}  // namespace OIConstants
