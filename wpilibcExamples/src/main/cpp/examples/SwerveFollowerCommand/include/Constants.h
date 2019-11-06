/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <units/units.h>

#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/trajectory/TrapezoidProfile.h>

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
constexpr int kFrontLeftDriveMotorPort = 0;
constexpr int kRearLeftDriveMotorPort = 2;
constexpr int kFrontRightDriveMotorPort = 4;
constexpr int kRearRightDriveMotorPort = 6;

constexpr int kFrontLeftTurningMotorPort = 1;
constexpr int kRearLeftTurningMotorPort = 3;
constexpr int kFrontRightTurningMotorPort = 5;
constexpr int kRearRightTurningMotorPort = 7;

constexpr std::array kFrontLeftTurningEncoderPorts = {0, 1};
constexpr std::array kRearLeftTurningEncoderPorts = {2, 3};
constexpr std::array kFrontRightTurningEncoderPorts = {4, 5};
constexpr std::array kRearRightTurningEncoderPorts = {5, 6};

constexpr bool kFrontLeftTurningEncoderReversed = false;
constexpr bool kRearLeftTurningEncoderReversed = true;
constexpr bool kFrontRightTurningEncoderReversed = false;
constexpr bool kRearRightTurningEncoderReversed = true;

constexpr std::array kFrontLeftDriveEncoderPorts = {0, 1};
constexpr std::array kRearLeftDriveEncoderPorts = {2, 3};
constexpr std::array kFrontRightDriveEncoderPorts = {4, 5};
constexpr std::array kRearRightDriveEncoderPorts = {5, 6};

constexpr bool kFrontLeftDriveEncoderReversed = false;
constexpr bool kRearLeftDriveEncoderReversed = true;
constexpr bool kFrontRightDriveEncoderReversed = false;
constexpr bool kRearRightDriveEncoderReversed = true;

constexpr auto kTrackWidth =
    .5_m;  // Distance between centers of right and left wheels on robot
constexpr auto kTrackLength =
    .7_m;  // Distance between centers of front and back wheels on robot

constexpr bool kGyroReversed = false;

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The RobotPy Characterization
// Toolsuite provides a convenient tool for obtaining these values for your
// robot.
constexpr auto ks = 1_V;
constexpr auto kv = .8 * 1_V * 1_s / 1_m;
constexpr auto ka = .15 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
constexpr double kPFrontLeftVel = .5;
constexpr double kPRearLeftVel = .5;
constexpr double kPFrontRightVel = .5;
constexpr double kPRearRightVel = .5;
}  // namespace DriveConstants

namespace ModuleConstants {
constexpr int kEncoderCPR = 1024;
constexpr double kWheelDiameterMeters = .15;
constexpr double kDriveEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterMeters * wpi::math::pi) / static_cast<double>(kEncoderCPR);

constexpr double kTurningEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (wpi::math::pi * 2) / static_cast<double>(kEncoderCPR);

constexpr double kPModuleTurningController = 1;
constexpr double kPModuleDriveController = 1;
}  // namespace ModuleConstants

namespace AutoConstants {
using radians_per_second_squared_t =
    units::compound_unit<units::radians,
                         units::inverse<units::squared<units::second>>>;

constexpr auto kMaxSpeed = units::meters_per_second_t(3);
constexpr auto kMaxAcceleration = units::meters_per_second_squared_t(3);
constexpr auto kMaxAngularSpeed = units::radians_per_second_t(3.142);
constexpr auto kMaxAngularAcceleration =
    units::unit_t<radians_per_second_squared_t>(3.142);

constexpr double kPXController = .5;
constexpr double kPYController = .5;
constexpr double kPThetaController = .5;

//

constexpr frc::TrapezoidProfile::Constraints kThetaControllerConstraints{
    units::meters_per_second_t(kMaxAngularSpeed.to<double>()),
    units::meters_per_second_squared_t(kMaxAngularAcceleration.to<double>())};

}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 1;
}  // namespace OIConstants