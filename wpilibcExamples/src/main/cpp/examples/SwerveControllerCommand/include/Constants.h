// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/TimedRobot.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>

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
inline constexpr int kFrontLeftDriveMotorPort = 0;
inline constexpr int kRearLeftDriveMotorPort = 2;
inline constexpr int kFrontRightDriveMotorPort = 4;
inline constexpr int kRearRightDriveMotorPort = 6;

inline constexpr int kFrontLeftTurningMotorPort = 1;
inline constexpr int kRearLeftTurningMotorPort = 3;
inline constexpr int kFrontRightTurningMotorPort = 5;
inline constexpr int kRearRightTurningMotorPort = 7;

inline constexpr int kFrontLeftTurningEncoderPorts[2]{0, 1};
inline constexpr int kRearLeftTurningEncoderPorts[2]{2, 3};
inline constexpr int kFrontRightTurningEncoderPorts[2]{4, 5};
inline constexpr int kRearRightTurningEncoderPorts[2]{6, 7};

inline constexpr bool kFrontLeftTurningEncoderReversed = false;
inline constexpr bool kRearLeftTurningEncoderReversed = true;
inline constexpr bool kFrontRightTurningEncoderReversed = false;
inline constexpr bool kRearRightTurningEncoderReversed = true;

inline constexpr int kFrontLeftDriveEncoderPorts[2]{8, 9};
inline constexpr int kRearLeftDriveEncoderPorts[2]{10, 11};
inline constexpr int kFrontRightDriveEncoderPorts[2]{12, 13};
inline constexpr int kRearRightDriveEncoderPorts[2]{14, 15};

inline constexpr bool kFrontLeftDriveEncoderReversed = false;
inline constexpr bool kRearLeftDriveEncoderReversed = true;
inline constexpr bool kFrontRightDriveEncoderReversed = false;
inline constexpr bool kRearRightDriveEncoderReversed = true;

// If you call DriveSubsystem::Drive with a different period make sure to update
// this.
inline constexpr units::second_t kDrivePeriod = frc::TimedRobot::kDefaultPeriod;

// These are example values only - DO NOT USE THESE FOR YOUR OWN ROBOT!
// These characterization values MUST be determined either experimentally or
// theoretically for *your* robot's drive. The SysId tool provides a convenient
// method for obtaining these values for your robot.
inline constexpr auto ks = 1_V;
inline constexpr auto kv = 0.8 * 1_V * 1_s / 1_m;
inline constexpr auto ka = 0.15 * 1_V * 1_s * 1_s / 1_m;

// Example value only - as above, this must be tuned for your drive!
inline constexpr double kPFrontLeftVel = 0.5;
inline constexpr double kPRearLeftVel = 0.5;
inline constexpr double kPFrontRightVel = 0.5;
inline constexpr double kPRearRightVel = 0.5;
}  // namespace DriveConstants

namespace ModuleConstants {
inline constexpr int kEncoderCPR = 1024;
inline constexpr auto kWheelDiameter = 0.15_m;
inline constexpr double kDriveEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameter.value() * std::numbers::pi) /
    static_cast<double>(kEncoderCPR);

inline constexpr double kTurningEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (std::numbers::pi * 2) / static_cast<double>(kEncoderCPR);

inline constexpr double kPModuleTurningController = 1;
inline constexpr double kPModuleDriveController = 1;
}  // namespace ModuleConstants

namespace AutoConstants {
inline constexpr auto kMaxSpeed = 3_mps;
inline constexpr auto kMaxAcceleration = 3_mps_sq;
inline constexpr auto kMaxAngularSpeed = 3.142_rad_per_s;
inline constexpr auto kMaxAngularAcceleration = 3.142_rad_per_s_sq;

inline constexpr double kPXController = 0.5;
inline constexpr double kPYController = 0.5;
inline constexpr double kPThetaController = 0.5;

//

extern const frc::TrapezoidProfile<units::radians>::Constraints
    kThetaControllerConstraints;

}  // namespace AutoConstants

namespace OIConstants {
inline constexpr int kDriverControllerPort = 0;
}  // namespace OIConstants
