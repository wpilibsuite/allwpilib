/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h>
#include <units/acceleration.h>
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>
#include <wpi/math>

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

constexpr auto kTrackwidth = 0.69_m;
extern const frc::DifferentialDriveKinematics kDriveKinematics;

constexpr int kEncoderCPR = 1024;
constexpr double kWheelDiameterInches = 6;
constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * wpi::math::pi) / static_cast<double>(kEncoderCPR);

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The Robot Characterization
// Toolsuite provides a convenient tool for obtaining these values for your
// robot.
constexpr auto ks = 0.22_V;
constexpr auto kv = 1.98 * 1_V * 1_s / 1_m;
constexpr auto ka = 0.2 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
constexpr double kPDriveVel = 8.5;
}  // namespace DriveConstants

namespace AutoConstants {
constexpr auto kMaxSpeed = 3_mps;
constexpr auto kMaxAcceleration = 3_mps_sq;

// Reasonable baseline values for a RAMSETE follower in units of meters and
// seconds
constexpr double kRamseteB = 2;
constexpr double kRamseteZeta = 0.7;
}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 1;
}  // namespace OIConstants
