/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <units/units.h>

#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h>

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

const auto kTrackwidth = .6_m;
const frc::DifferentialDriveKinematics kDriveKinematics(kTrackwidth);

const int kEncoderCPR = 1024;
const double kWheelDiameterInches = 6;
const double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * 3.142) / static_cast<double>(kEncoderCPR);

const bool kGyroReversed = true;

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The RobotPy Characterization
// Toolsuite provides a convenient tool for obtaining these values for your
// robot.
const auto ks = 1_V;
const auto kv = .8 * 1_V * 1_s / 1_m;
const auto ka = .15 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
const double kPDriveVel = .5;
}  // namespace DriveConstants

namespace AutoConstants {
const auto kMaxSpeed = 3_mps;
const auto kMaxAcceleration = 3_m / (1_s * 1_s);

// Reasonable baseline values for a RAMSETE follower in units of meters and
// seconds
const double kRamseteB = 2;
const double kRamseteZeta = .7;
}  // namespace AutoConstants

namespace OIConstants {
const int kDriverControllerPort = 1;
}  // namespace OIConstants
